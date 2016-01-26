#include <jni.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>

#include "CommonLibrary/String.h"
#include "CommonLibrary/SpatialKey.h"
#include "CommonLibrary/Interlocked.h"

extern "C"	jstring
/*JNIEXPORT jstring JNICALL */Java_com_example_ndksample_app_MyNDK_getMyString (JNIEnv * env, jobject)
{
    CommonLib::CString str;
    str = "Test";
	int value = 0;
	 CommonLib::Interlocked::Increment(&value);
    return  (*env).NewStringUTF(str.cstr());
}
