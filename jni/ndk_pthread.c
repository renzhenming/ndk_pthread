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
	//��ȡclass����Ҫ�����߳���
	jclass uuidutils_class_tmp = (*env)->FindClass(env,"com/example/ndk_pthread/UUIDUtils");
	//����ȫ������
	uuid_class = (*env)->NewGlobalRef(env,uuidutils_class_tmp);
	//��ȡjmethodIdҲ���������߳���
	uuid_get_methodid = (*env)->GetStaticMethodID(env,uuid_class,"get","()Ljava/lang/String;");
}


JNIEXPORT void JNICALL Java_com_example_ndk_1pthread_PosixUtils_destroy
(JNIEnv *env, jobject jobj){
	//�ͷ�ȫ������
	(*env)->DeleteGlobalRef(env,uuid_class);
}

JNIEXPORT void JNICALL Java_com_example_ndk_1pthread_PosixUtils_pthread
  (JNIEnv *env, jobject jobj){
	LOGI("%s","begin");
	//��ȡJavaVM�ĵ�һ�ַ�ʽ������Ŀ�����ǲ�����JNI_OnLoad�л�ȡ�ķ�ʽ
	//(*env)->GetJavaVM(env,&javaVM);
	pthread_t tid;
	pthread_create(&tid,NULL,thread_fun,(void*)"NO1");
}
