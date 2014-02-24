package com.alexzhou.superman;

import android.app.NativeActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.ViewGroup.LayoutParams;
import android.view.ViewGroup.MarginLayoutParams;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.PopupWindow;

import com.google.ads.AdRequest;
import com.google.ads.AdSize;
import com.google.ads.AdView;

public class SuperManActivity extends NativeActivity {

	static AdView adView;

	static SuperManActivity _activity;
	static PopupWindow popUp;
	static LinearLayout mainLayout;
	static boolean adsinited = false;

	public void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);

		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		_activity = this;
		adView = new AdView(_activity, AdSize.BANNER, "a1530b06e52d0ca");
	}


	private static void showAdPopup() {
		Log.d("supermanactivity", "adsinited=" + adsinited);
		if (adsinited) {
			return;
		}

		if (adView != null) {
			_activity.runOnUiThread(new Runnable() {
				@Override
				public void run() {
					Log.d("supermanactivity ", "runOnUiThread");
					adsinited = true;
					// Out popup window

					popUp = new PopupWindow(_activity);

					// This is the minimum size for AdMob, we need to set this
					// in case our target device run at 320x480 resolution
					// (Otherwise no ad will be shown, see the padding kill
					// below)
					int screenWidth = _activity.getWindowManager()
							.getDefaultDisplay().getWidth();
					popUp.setWidth(screenWidth);

					popUp.setHeight(50);

					popUp.setWindowLayoutMode(LayoutParams.WRAP_CONTENT,
							LayoutParams.WRAP_CONTENT);

					popUp.setClippingEnabled(false);

					LinearLayout layout = new LinearLayout(_activity);

					mainLayout = new LinearLayout(_activity);

					// The layout system for the PopupWindow will kill some
					// pixels due to margins/paddings etc¡­ (No way to remove
					// it), so padd it to adjust

					layout.setPadding(0, 0, 0, 0);

					MarginLayoutParams params = new MarginLayoutParams(
							LayoutParams.FILL_PARENT,
							LayoutParams.FILL_PARENT);

					params.setMargins(0, 0, 0, 0);

					layout.setOrientation(LinearLayout.VERTICAL);

					layout.addView(adView, params);

					popUp.setContentView(layout);

					_activity.setContentView(mainLayout, params);

					AdRequest adRequest = new AdRequest();

					// Enable this if your are testing AdMob, otherwise you'll
					// risk to be banned!

					// adRequest.addTestDevice(AdRequest.TEST_EMULATOR);

					_activity.adView.loadAd(adRequest);

					// Show our popup window

					popUp.showAtLocation(mainLayout, Gravity.TOP, 0, 0);

					popUp.update();
				}
			});

		}

	}

	// Do some cleanup

	@Override
	public void onDestroy() {

		if (adView != null) {

			adView.destroy();
		}

		super.onDestroy();

	}

	public static void changeAdShow(final boolean show) {
		Log.d("SuperManActivity", "show=" + show + "; adsinited=" + adsinited);

		if (!adsinited) {
			showAdPopup();
		} else {
			_activity.runOnUiThread(new Runnable() {

				@Override
				public void run() {
					if (!show) {
						popUp.dismiss();
					} else {
						popUp.showAtLocation(mainLayout, Gravity.TOP, 0, 0);
						popUp.update();
					}

				}
			});
		}

	}
}