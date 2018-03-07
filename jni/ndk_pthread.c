#include "com_example_ndk_pthread_PosixUtils.h"
#include <stdio.h>
#include <pthread.h>
#include <android/log.h>
#include <unistd.h>
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"renzhenming",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"renzhenming",FORMAT,##__VA_ARGS__);

JavaVM *javaVM;
jobject uuid_class;
jmethodID uuid_get_methodid;

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
		jobject uuid = (*env)->CallStaticObjectMethod(env,uuid_class,uuid_get_methodid);
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

JNIEXPORT void JNICALL Java_com_example_ndk_1pthread_PosixUtils_init
(JNIEnv *env, jobject jobj){
	//获取class必须要在主线程中
	jclass uuidutils_class_tmp = (*env)->FindClass(env,"com/example/ndk_pthread/UUIDUtils");
	//创建全局引用
	uuid_class = (*env)->NewGlobalRef(env,uuidutils_class_tmp);
	//获取jmethodId也可以在子线程中
	uuid_get_methodid = (*env)->GetStaticMethodID(env,uuid_class,"get","()Ljava/lang/String;");
}


JNIEXPORT void JNICALL Java_com_example_ndk_1pthread_PosixUtils_destroy
(JNIEnv *env, jobject jobj){
	//释放全局引用
	(*env)->DeleteGlobalRef(env,uuid_class);
}

JNIEXPORT void JNICALL Java_com_example_ndk_1pthread_PosixUtils_pthread
  (JNIEnv *env, jobject jobj){
	LOGI("%s","begin");
	//获取JavaVM的第一种方式，本项目中我们采用在JNI_OnLoad中获取的方式
	//(*env)->GetJavaVM(env,&javaVM);
	pthread_t tid;
	pthread_create(&tid,NULL,thread_fun,(void*)"NO1");
}
