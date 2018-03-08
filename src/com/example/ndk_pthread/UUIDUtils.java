package com.example.ndk_pthread;

import java.util.UUID;

import android.content.Context;
import android.widget.Toast;

public class UUIDUtils {

	public static String get(){
		return UUID.randomUUID().toString();
	}
	
	public static void showToast(Context context){
		Toast.makeText(context, "dsfsdfds", 0).show();
	}
	
}
