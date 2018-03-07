package com.example.ndk_pthread;

public class PosixUtils {
	
	static{
		System.loadLibrary("ndk_pthread");
	}
	public native void init();
	
	public native void destroy();
	public native void pthread(); 
}
