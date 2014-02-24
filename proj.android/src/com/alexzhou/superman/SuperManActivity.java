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
	static int screenWidth = 720;

	public void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);

		// Make your custom init here

		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		_activity = this;

		// Create our ad view here

		adView = new AdView(_activity, AdSize.BANNER, "a1530b06e52d0ca");

		screenWidth = this.getWindowManager().getDefaultDisplay().getWidth();
	}

	// Our popup window, you will call it from your C/C++ code later

	private static void showAdPopup()

	{
		Log.d("supermanactivity", "adsinited=" + adsinited);
		if (adsinited)

		{
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

					//layout.setPadding(-5, -5, -5, -5);

					MarginLayoutParams params = new MarginLayoutParams(
							LayoutParams.WRAP_CONTENT,
							LayoutParams.WRAP_CONTENT);

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
	
	public static void changeAdShow(boolean show) {
		if(!adsinited) {
			showAdPopup();
		}else {
			if(!show) {
				popUp.dismiss();
				popUp = null;
				mainLayout = null;
				adsinited = false;
			}
		}

	}
}