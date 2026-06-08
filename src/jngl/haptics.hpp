// Copyright 2026 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Functions to control haptic feedback (vibration)
/// @file
#pragma once

#include <cstdint>

namespace jngl {

/// Type of haptic feedback to trigger via jngl::hapticFeedback
enum class HapticFeedback : uint8_t {
	/// Short, light feedback suitable for pressing a button or virtual key
	Light,
	/// Medium feedback, e.g. for a toggle or selection change
	Medium,
	/// Strong feedback, e.g. for a long press or an important event
	Heavy,
	/// A long (~300 ms) vibration that starts strong and fades out, e.g. for an error or game over
	Long,
};

/// Triggers haptic feedback (a short vibration)
///
/// Currently only implemented on Android, where it plays a tuned VibrationEffect.Composition (with a
/// fallback to View.performHapticFeedback on older devices). This requires
/// `<uses-permission android:name="android.permission.VIBRATE" />` in your AndroidManifest.xml. On
/// all other platforms this function does nothing.
void hapticFeedback(HapticFeedback type = HapticFeedback::Light);

} // namespace jngl
