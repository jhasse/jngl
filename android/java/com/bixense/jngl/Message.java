package com.bixense.jngl;

import android.app.Activity;
import android.app.AlertDialog;

/**
 * Java side of jngl::errorMessage(), called from src/android/message.cpp.
 *
 * A dialog can only be created on Android's UI thread and hopping there needs a
 * Runnable, which JNI can't create on its own — hence this helper instead of
 * doing everything natively. Add this directory to your app's Java sourceSets to
 * get error dialogs:
 *
 * <pre>
 * android.sourceSets.main.java.srcDirs += "path/to/jngl/android/java"
 * </pre>
 *
 * Apps which don't do that still get the message on logcat.
 */
public final class Message {
	private Message() {
	}

	/** Shows \a text in a dismissable dialog on top of \a activity. */
	public static void error(Activity activity, String text) {
		activity.runOnUiThread(() -> {
			if (activity.isFinishing() || activity.isDestroyed()) {
				return; // no window to attach the dialog to
			}
			new AlertDialog.Builder(activity)
			    .setTitle("Error")
			    .setMessage(text)
			    .setPositiveButton(android.R.string.ok, null)
			    .show();
		});
	}
}
