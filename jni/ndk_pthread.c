#include "com_example_ndk_pthread_PosixUtils.h"
#include <stdio.h>
#include <pthread.h>
#include <android/log.h>
#include <unistd.h>
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"renzhenming",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"renzhenming",FORMAT,##__VA_ARGS__);

JavaVM *javaVM;
jobject uuidutils_jcls;
jmethodID get_mid;
jmethodID toastId;
jobject jcontext;
//动态库加载时会被调用执行，不需要我们手动调用
//兼容Android SDK 2.2之后，2.2没有这个函数
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){
	LOGI("%s","JNI_OnLoad");
	javaVM = vm;
	return JNI_VERSION_1_4;
}

/**
 * 目标:调用UUIDUtils类中的方法得到一个uuid
 */
void thread_fun(void* arg){
	//得到这个类UUIDUtils,需要用到JNIEnv,但是这是一个子线程，每个线程都有独立的JNIEnv，所以我们需要获取到
	//这个线程的JNIEnv，通过JavaVM关联当前线程，获取当前线程的JNIEnv，(*javaVM)->AttachCurrentThread(javaVM,&env,NULL);
	//那么就需要先获取到JavaVM
	//如何获取JavaVM？
	//1.在JNI_OnLoad函数中获取
	//2.(*env)->GetJavaVM(env,&javaVM);

	//的到JavaVM后,会得到env并赋值给变量
	JNIEnv* env = NULL;
	(*javaVM)->AttachCurrentThread(javaVM,&env,NULL);

	char* no = (char*)arg;
	int i;
	for (i = 0; i < 5; ++i) {
		LOGI("thread %s, i:%d",no,i);
		jobject uuid = (*env)->CallStaticObjectMethod(env,uuidutils_jcls,get_mid);



		char* uuid_char = (*env)->GetStringUTFChars(env,uuid,NULL);


		LOGI("%s",uuid_char);
		if(i == 4){
			pthread_exit((void*)0);
		}
		(*env)->ReleaseStringUTFChars(env,uuid,uuid_char);
		sleep(1);
	}
	(*javaVM)->DetachCurrentThread(javaVM);
}
/**
 * 当在JNI调用Android自带的类时，经常需要传入Context参数，那怎么在JNI层获取Context呢？
 * 我们知道Application和Activity是Context的子类，由于每个Activity对应的Context是不一样的，
 * 所以一般情况下我们使用Application的Context，它在整个程序中只有一个实例。所以现在问题就变成了
 * 怎么在JNI中获取Application呢？
 * Android APP在启动时会创建一个Activity Thread作为主线程，只要程序存活，这个线程就一直存在，
 * 所以我们可以考虑从Activity Thread中获取Application，查看Activity Thread的源码发现，
 * 它提供了一个方法可以获取Application，如下：
 *
 * public Application getApplication() {
 *    return mInitialApplication;
 * }
 *
 * 也就是说我们只需要获取到Activity Thread的对象即可，Activity Thread提供了一个静态方法用于获取其实例，如下：
 *
 * public static ActivityThread currentActivityThread() {
 *    return sCurrentActivityThread;
 * }
 *
 * 至此获取Context的步骤已经很清晰了
 */
JNIEXPORT void JNICALL Java_com_example_ndk_1pthread_PosixUtils_init
(JNIEnv *env, jobject jobj){
	/**
	 * 打印uuid
	 */
	//获取class必须要在主线程中
	jclass uuidutils_class_tmp = (*env)->FindClass(env,"com/example/ndk_pthread/UUIDUtils");
	//创建全局引用
	uuidutils_jcls = (*env)->NewGlobalRef(env,uuidutils_class_tmp);
	//获取jmethodId也可以在子线程中
	get_mid = (*env)->GetStaticMethodID(env,uuidutils_jcls,"get","()Ljava/lang/String;");
	/**
	 * show toast
	 */
	//获取jmethodId也可以在子线程中
	toastId = (*env)->GetStaticMethodID(env,uuidutils_jcls,"showToast","(Landroid/content/Context;)V");

	//获取Activity Thread的实例对象
	jclass activityThread = (*env)->FindClass(env,"android/app/ActivityThread");
	jmethodID currentActivityThread = (*env)->GetStaticMethodID(env,activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");
	jobject at = (*env)->CallStaticObjectMethod(env,activityThread, currentActivityThread);
	//获取Application，也就是全局的Context
	jmethodID getApplication = (*env)->GetMethodID(env,activityThread, "getApplication", "()Landroid/app/Application;");
	jobject context = (*env)->CallObjectMethod(env,at, getApplication);

	jcontext = (*env)->NewGlobalRef(env,context);
}


JNIEXPORT void JNICALL Java_com_example_ndk_1pthread_PosixUtils_destroy
(JNIEnv *env, jobject jobj){
	//释放全局引用
	(*env)->DeleteGlobalRef(env,uuidutils_jcls);
}

JNIEXPORT void JNICALL Java_com_example_ndk_1pthread_PosixUtils_pthread
  (JNIEnv *env, jobject jobj){
	LOGI("%s","begin");
	//获取JavaVM的第一种方式，本项目中我们采用在JNI_OnLoad中获取的方式
	//(*env)->GetJavaVM(env,&javaVM);
	pthread_t tid;
	pthread_create(&tid,NULL,thread_fun,(void*)"NO1");

	//调用Java函数show toast
	(*env)->CallStaticVoidMethod(env,uuidutils_jcls,toastId,jcontext);
}
