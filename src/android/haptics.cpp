// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "../jngl/haptics.hpp"

#include "../jngl/Finally.hpp"
#include "windowimpl.hpp"

#include <algorithm>
#include <android_native_app_glue.h>
#include <vector>

namespace jngl {

extern android_app* androidApp;

namespace {

JNIEnv* getEnv() {
	return reinterpret_cast<WindowImpl*>(androidApp->userData)->env;
}

int getSdkInt(JNIEnv* env) {
	jclass versionClass = env->FindClass("android/os/Build$VERSION");
	Finally releaseVersionClass([&]() { env->DeleteLocalRef(versionClass); });
	return env->GetStaticIntField(versionClass,
	                              env->GetStaticFieldID(versionClass, "SDK_INT", "I"));
}

// VibrationEffect.Composition primitive IDs, see
// https://developer.android.com/reference/android/os/VibrationEffect.Composition
constexpr jint PRIMITIVE_CLICK = 1;
constexpr jint PRIMITIVE_TICK = 7;

// A single step in a VibrationEffect.Composition.
struct CompositionStep {
	jint primitiveId;
	jfloat scale;
	jint delayMs; // delay since the end of the previous primitive
};

// The composition played for a given feedback type. We deliberately chain Android's tuned haptic
// primitives instead of driving the actuator with a raw amplitude waveform: on devices with a good
// actuator (e.g. Pixel) the primitives feel noticeably crisper ("more premium").
std::vector<CompositionStep> compositionSteps(HapticFeedback type) {
	switch (type) {
	case HapticFeedback::Light:
		return { { PRIMITIVE_TICK, 0.5f, 0 } };
	case HapticFeedback::Medium:
		return { { PRIMITIVE_CLICK, 0.6f, 0 } };
	case HapticFeedback::Heavy:
		return { { PRIMITIVE_CLICK, 1.0f, 0 } };
	case HapticFeedback::Long:
		// A strong click followed by ticks of decreasing intensity, fading out over ~300 ms.
		return {
			{ PRIMITIVE_CLICK, 1.0f, 0 },  { PRIMITIVE_TICK, 0.6f, 45 },
			{ PRIMITIVE_TICK, 0.4f, 45 },  { PRIMITIVE_TICK, 0.25f, 45 },
			{ PRIMITIVE_TICK, 0.15f, 45 },
		};
	}
	return { { PRIMITIVE_CLICK, 1.0f, 0 } };
}

// HapticFeedbackConstants used by the pre-API-31 fallback, see
// https://developer.android.com/reference/android/view/HapticFeedbackConstants
jint toHapticFeedbackConstant(HapticFeedback type) {
	switch (type) {
	case HapticFeedback::Light:
		return 1; // VIRTUAL_KEY
	case HapticFeedback::Medium:
		return 6; // CONTEXT_CLICK
	case HapticFeedback::Heavy:
	case HapticFeedback::Long:
		return 0; // LONG_PRESS
	}
	return 1;
}

// Builds a VibrationEffect.Composition for the given type, or returns nullptr if the device doesn't
// support all of the primitives it uses. The returned local reference must be freed by the caller.
jobject createCompositionEffect(JNIEnv* env, jclass effectClass, jobject vibrator,
                                jclass vibratorClass, HapticFeedback type) {
	const std::vector<CompositionStep> steps = compositionSteps(type);

	// Verify the device supports every (distinct) primitive we're about to use.
	std::vector<jint> ids;
	for (const CompositionStep& step : steps) {
		if (std::find(ids.begin(), ids.end(), step.primitiveId) == ids.end()) {
			ids.push_back(step.primitiveId);
		}
	}
	jintArray idArray = env->NewIntArray(static_cast<jsize>(ids.size()));
	Finally releaseIdArray([&]() { env->DeleteLocalRef(idArray); });
	env->SetIntArrayRegion(idArray, 0, static_cast<jsize>(ids.size()), ids.data());
	jmethodID areAllPrimitivesSupported =
	    env->GetMethodID(vibratorClass, "areAllPrimitivesSupported", "([I)Z");
	if (!env->CallBooleanMethod(vibrator, areAllPrimitivesSupported, idArray)) {
		return nullptr;
	}

	jmethodID startComposition = env->GetStaticMethodID(
	    effectClass, "startComposition", "()Landroid/os/VibrationEffect$Composition;");
	jobject composition = env->CallStaticObjectMethod(effectClass, startComposition);
	Finally releaseComposition([&]() { env->DeleteLocalRef(composition); });

	jclass compositionClass = env->GetObjectClass(composition);
	Finally releaseCompositionClass([&]() { env->DeleteLocalRef(compositionClass); });
	jmethodID addPrimitive = env->GetMethodID(compositionClass, "addPrimitive",
	                                          "(IFI)Landroid/os/VibrationEffect$Composition;");
	for (const CompositionStep& step : steps) {
		// addPrimitive returns the same builder, so we don't need to keep its local reference.
		env->DeleteLocalRef(env->CallObjectMethod(composition, addPrimitive, step.primitiveId,
		                                          step.scale, step.delayMs));
	}

	jmethodID compose =
	    env->GetMethodID(compositionClass, "compose", "()Landroid/os/VibrationEffect;");
	return env->CallObjectMethod(composition, compose);
}

// Plays haptic feedback using the modern Vibrator + VibrationEffect.Composition API which feels
// crisp and lets us control the intensity. Returns false if it's not available (older Android
// version or an unsupported primitive), so that the caller can fall back.
bool vibrateWithEffect(JNIEnv* env, HapticFeedback type) {
	if (getSdkInt(env) < 31) { // VibratorManager was added in Android 12 (API 31)
		return false;
	}

	jclass activityClass = env->GetObjectClass(androidApp->activity->clazz);
	Finally releaseActivityClass([&]() { env->DeleteLocalRef(activityClass); });
	jmethodID getSystemService = env->GetMethodID(activityClass, "getSystemService",
	                                              "(Ljava/lang/String;)Ljava/lang/Object;");
	jstring serviceName = env->NewStringUTF("vibrator_manager"); // Context.VIBRATOR_MANAGER_SERVICE
	Finally releaseServiceName([&]() { env->DeleteLocalRef(serviceName); });
	jobject vibratorManager =
	    env->CallObjectMethod(androidApp->activity->clazz, getSystemService, serviceName);
	if (!vibratorManager) {
		return false;
	}
	Finally releaseVibratorManager([&]() { env->DeleteLocalRef(vibratorManager); });

	jclass vibratorManagerClass = env->GetObjectClass(vibratorManager);
	Finally releaseVibratorManagerClass([&]() { env->DeleteLocalRef(vibratorManagerClass); });
	jmethodID getDefaultVibrator =
	    env->GetMethodID(vibratorManagerClass, "getDefaultVibrator", "()Landroid/os/Vibrator;");
	jobject vibrator = env->CallObjectMethod(vibratorManager, getDefaultVibrator);
	if (!vibrator) {
		return false;
	}
	Finally releaseVibrator([&]() { env->DeleteLocalRef(vibrator); });

	jclass vibratorClass = env->GetObjectClass(vibrator);
	Finally releaseVibratorClass([&]() { env->DeleteLocalRef(vibratorClass); });

	jclass effectClass = env->FindClass("android/os/VibrationEffect");
	Finally releaseEffectClass([&]() { env->DeleteLocalRef(effectClass); });

	jobject effect = createCompositionEffect(env, effectClass, vibrator, vibratorClass, type);
	if (!effect) {
		return false;
	}
	Finally releaseEffect([&]() { env->DeleteLocalRef(effect); });

	jmethodID vibrate =
	    env->GetMethodID(vibratorClass, "vibrate", "(Landroid/os/VibrationEffect;)V");
	env->CallVoidMethod(vibrator, vibrate, effect);
	return true;
}

// Fallback for older devices: View.performHapticFeedback respects the user's system haptic settings
// and needs no permission, but its semantic constants feel similar on many devices.
void performHapticFeedback(JNIEnv* env, HapticFeedback type) {
	jmethodID getWindow = env->GetMethodID(env->GetObjectClass(androidApp->activity->clazz),
	                                       "getWindow", "()Landroid/view/Window;");
	jobject window = env->CallObjectMethod(androidApp->activity->clazz, getWindow);
	if (!window) {
		return;
	}
	Finally releaseWindow([&]() { env->DeleteLocalRef(window); });

	jmethodID getDecorView =
	    env->GetMethodID(env->GetObjectClass(window), "getDecorView", "()Landroid/view/View;");
	jobject decorView = env->CallObjectMethod(window, getDecorView);
	if (!decorView) {
		return;
	}
	Finally releaseDecorView([&]() { env->DeleteLocalRef(decorView); });

	jmethodID perform =
	    env->GetMethodID(env->GetObjectClass(decorView), "performHapticFeedback", "(I)Z");
	env->CallBooleanMethod(decorView, perform, toHapticFeedbackConstant(type));
}

} // namespace

void hapticFeedback(HapticFeedback type) {
	JNIEnv* env = getEnv();
	if (vibrateWithEffect(env, type)) {
		return;
	}
	performHapticFeedback(env, type);
}

} // namespace jngl
