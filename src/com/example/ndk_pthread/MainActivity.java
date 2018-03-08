package com.example.ndk_pthread;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

public class MainActivity extends Activity {

	private PosixUtils p;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		p = new PosixUtils();
		p.init();
	}

	public void start(View btn){
		p.pthread();
	}
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
		p.destroy();
	}

}
