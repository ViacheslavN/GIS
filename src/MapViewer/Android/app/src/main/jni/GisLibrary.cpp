#include <jni.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>

#include "CommonLibrary/String.h"
#include "CommonLibrary/SpatialKey.h"


JNIEXPORT jstring JNICALL Java_com_example_ndksample_app_MyNDK_getMyString (JNIEnv * env, jobject)
{
    CommonLib::CString str;
    str = "Test";

    return  (*env).NewStringUTF(str.cstr());
}
