// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "../jngl/haptics.hpp"

#import <TargetConditionals.h>

#if TARGET_OS_TV

// tvOS has no Taptic Engine: neither the UIKit feedback generators nor Core Haptics are available,
// so hapticFeedback() is a no-op there.
namespace jngl {

void hapticFeedback(HapticFeedback) {
}

} // namespace jngl

#else

#import <CoreHaptics/CoreHaptics.h>
#import <UIKit/UIKit.h>

namespace jngl {

namespace {

// UIImpactFeedbackGenerator instances are cached and kept "warm" so the Taptic Engine responds with
// minimal latency. They drive Apple's tuned feedback styles for the short Light/Medium/Heavy
// feedback, mirroring how the Android implementation chains tuned VibrationEffect primitives
// instead of a raw amplitude waveform. The generators live for the lifetime of the app, so they're
// never released.
UIImpactFeedbackGenerator* impactGenerator(HapticFeedback type) {
	static UIImpactFeedbackGenerator* light =
	    [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleLight];
	static UIImpactFeedbackGenerator* medium =
	    [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleMedium];
	static UIImpactFeedbackGenerator* heavy =
	    [[UIImpactFeedbackGenerator alloc] initWithStyle:UIImpactFeedbackStyleHeavy];
	switch (type) {
	case HapticFeedback::Light:
		return light;
	case HapticFeedback::Medium:
		return medium;
	default:
		return heavy;
	}
}

// Lazily created Core Haptics engine used for the Long fade-out pattern. Returns nil if the device
// doesn't support Core Haptics (e.g. the Simulator or an iPad without a Taptic Engine), so the
// caller can fall back. The engine is kept alive for the lifetime of the app.
CHHapticEngine* hapticEngine() {
	if (![CHHapticEngine capabilitiesForHardware].supportsHaptics) {
		return nil;
	}
	static CHHapticEngine* engine = nil;
	if (!engine) {
		NSError* error = nil;
		engine = [[CHHapticEngine alloc] initAndReturnError:&error];
		if (error || !engine) {
			[engine release];
			engine = nil;
			return nil;
		}
		// The engine stops itself when the app is backgrounded or its audio session is interrupted;
		// drop our reference so the next call recreates and restarts it.
		engine.stoppedHandler = ^(CHHapticEngineStoppedReason) {
		  [engine release];
		  engine = nil;
		};
		// If the engine resets (e.g. after a media server crash) it needs to be restarted before it
		// can play again.
		engine.resetHandler = ^{
		  NSError* startError = nil;
		  [engine startAndReturnError:&startError];
		};
	}
	return engine;
}

// Plays the ~300 ms fade-out used for HapticFeedback::Long. Instead of a continuous event (which
// reads as a cheap buzz) we chain a series of sharp transient taps with decreasing intensity, like
// the Android composition's leading strong CLICK followed by fading TICKs. Crisp transients feel
// distinctly more "haptic" and premium than a sustained vibration.
void playLong(CHHapticEngine* engine) {
	NSError* error = nil;
	if (![engine startAndReturnError:&error] || error) {
		return;
	}

	// { relative time (s), intensity, sharpness }. The first hit lands soft-but-strong, then the
	// taps get progressively quieter and crisper as they fade out.
	struct Tap {
		double time;
		float intensity;
		float sharpness;
	};
	static constexpr Tap taps[] = {
		{ 0.000, 1.00f, 0.5f }, { 0.055, 0.65f, 0.6f }, { 0.110, 0.42f, 0.7f },
		{ 0.165, 0.27f, 0.8f }, { 0.220, 0.16f, 0.9f },
	};

	NSMutableArray<CHHapticEvent*>* events = [NSMutableArray arrayWithCapacity:5];
	for (const Tap& tap : taps) {
		CHHapticEventParameter* intensity = [[[CHHapticEventParameter alloc]
		    initWithParameterID:CHHapticEventParameterIDHapticIntensity
		                  value:tap.intensity] autorelease];
		CHHapticEventParameter* sharpness = [[[CHHapticEventParameter alloc]
		    initWithParameterID:CHHapticEventParameterIDHapticSharpness
		                  value:tap.sharpness] autorelease];
		[events addObject:[[[CHHapticEvent alloc] initWithEventType:CHHapticEventTypeHapticTransient
		                                                 parameters:@[ intensity, sharpness ]
		                                               relativeTime:tap.time] autorelease]];
	}

	NSError* patternError = nil;
	CHHapticPattern* pattern = [[[CHHapticPattern alloc] initWithEvents:events
	                                                    parameterCurves:@[]
	                                                              error:&patternError] autorelease];
	if (patternError || !pattern) {
		return;
	}

	NSError* playerError = nil;
	id<CHHapticPatternPlayer> player = [engine createPlayerWithPattern:pattern error:&playerError];
	if (playerError || !player) {
		return;
	}
	[player startAtTime:0 error:nil];
}

} // namespace

void hapticFeedback(HapticFeedback type) {
	if (type == HapticFeedback::Long) {
		if (CHHapticEngine* engine = hapticEngine()) {
			playLong(engine);
			return;
		}
		// Core Haptics isn't available: fall back to the system error notification, which gives a
		// distinctive multi-pulse buzz that suits an error or game over.
		UINotificationFeedbackGenerator* generator =
		    [[[UINotificationFeedbackGenerator alloc] init] autorelease];
		[generator notificationOccurred:UINotificationFeedbackTypeError];
		return;
	}
	UIImpactFeedbackGenerator* generator = impactGenerator(type);
	[generator prepare];
	// Light is dialled down so it reads as a gentle tick rather than a full tap; Medium/Heavy play
	// at the generator's full strength.
	[generator impactOccurredWithIntensity:(type == HapticFeedback::Light ? 0.6f : 1.0f)];
}

} // namespace jngl

#endif
