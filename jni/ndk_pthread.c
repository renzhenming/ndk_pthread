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
//��̬�����ʱ�ᱻ����ִ�У�����Ҫ�����ֶ�����
//����Android SDK 2.2֮��2.2û���������
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){
	LOGI("%s","JNI_OnLoad");
	javaVM = vm;
	return JNI_VERSION_1_4;
}

/**
 * Ŀ��:����UUIDUtils���еķ����õ�һ��uuid
 */
void thread_fun(void* arg){
	//�õ������UUIDUtils,��Ҫ�õ�JNIEnv,��������һ�����̣߳�ÿ���̶߳��ж�����JNIEnv������������Ҫ��ȡ��
	//����̵߳�JNIEnv��ͨ��JavaVM������ǰ�̣߳���ȡ��ǰ�̵߳�JNIEnv��(*javaVM)->AttachCurrentThread(javaVM,&env,NULL);
	//��ô����Ҫ�Ȼ�ȡ��JavaVM
	//��λ�ȡJavaVM��
	//1.��JNI_OnLoad�����л�ȡ
	//2.(*env)->GetJavaVM(env,&javaVM);

	//�ĵ�JavaVM��,��õ�env����ֵ������
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
 * ����JNI����Android�Դ�����ʱ��������Ҫ����Context����������ô��JNI���ȡContext�أ�
 * ����֪��Application��Activity��Context�����࣬����ÿ��Activity��Ӧ��Context�ǲ�һ���ģ�
 * ����һ�����������ʹ��Application��Context����������������ֻ��һ��ʵ����������������ͱ����
 * ��ô��JNI�л�ȡApplication�أ�
 * Android APP������ʱ�ᴴ��һ��Activity Thread��Ϊ���̣߳�ֻҪ���������߳̾�һֱ���ڣ�
 * �������ǿ��Կ��Ǵ�Activity Thread�л�ȡApplication���鿴Activity Thread��Դ�뷢�֣�
 * ���ṩ��һ���������Ի�ȡApplication�����£�
 *
 * public Application getApplication() {
 *    return mInitialApplication;
 * }
 *
 * Ҳ����˵����ֻ��Ҫ��ȡ��Activity Thread�Ķ��󼴿ɣ�Activity Thread�ṩ��һ����̬�������ڻ�ȡ��ʵ�������£�
 *
 * public static ActivityThread currentActivityThread() {
 *    return sCurrentActivityThread;
 * }
 *
 * ���˻�ȡContext�Ĳ����Ѿ���������
 */
JNIEXPORT void JNICALL Java_com_example_ndk_1pthread_PosixUtils_init
(JNIEnv *env, jobject jobj){
	/**
	 * ��ӡuuid
	 */
	//��ȡclass����Ҫ�����߳���
	jclass uuidutils_class_tmp = (*env)->FindClass(env,"com/example/ndk_pthread/UUIDUtils");
	//����ȫ������
	uuidutils_jcls = (*env)->NewGlobalRef(env,uuidutils_class_tmp);
	//��ȡjmethodIdҲ���������߳���
	get_mid = (*env)->GetStaticMethodID(env,uuidutils_jcls,"get","()Ljava/lang/String;");
	/**
	 * show toast
	 */
	//��ȡjmethodIdҲ���������߳���
	toastId = (*env)->GetStaticMethodID(env,uuidutils_jcls,"showToast","(Landroid/content/Context;)V");

	//��ȡActivity Thread��ʵ������
	jclass activityThread = (*env)->FindClass(env,"android/app/ActivityThread");
	jmethodID currentActivityThread = (*env)->GetStaticMethodID(env,activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");
	jobject at = (*env)->CallStaticObjectMethod(env,activityThread, currentActivityThread);
	//��ȡApplication��Ҳ����ȫ�ֵ�Context
	jmethodID getApplication = (*env)->GetMethodID(env,activityThread, "getApplication", "()Landroid/app/Application;");
	jobject context = (*env)->CallObjectMethod(env,at, getApplication);

	jcontext = (*env)->NewGlobalRef(env,context);
}


JNIEXPORT void JNICALL Java_com_example_ndk_1pthread_PosixUtils_destroy
(JNIEnv *env, jobject jobj){
	//�ͷ�ȫ������
	(*env)->DeleteGlobalRef(env,uuidutils_jcls);
}

JNIEXPORT void JNICALL Java_com_example_ndk_1pthread_PosixUtils_pthread
  (JNIEnv *env, jobject jobj){
	LOGI("%s","begin");
	//��ȡJavaVM�ĵ�һ�ַ�ʽ������Ŀ�����ǲ�����JNI_OnLoad�л�ȡ�ķ�ʽ
	//(*env)->GetJavaVM(env,&javaVM);
	pthread_t tid;
	pthread_create(&tid,NULL,thread_fun,(void*)"NO1");

	//����Java����show toast
	(*env)->CallStaticVoidMethod(env,uuidutils_jcls,toastId,jcontext);
}
