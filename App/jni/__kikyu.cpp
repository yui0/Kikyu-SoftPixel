#include <jni.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "threaded_app", __VA_ARGS__))

void android_main(struct android_app* App)
{
	LOGI("Load OK!!");
}
