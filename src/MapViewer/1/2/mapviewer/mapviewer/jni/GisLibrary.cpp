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
/*
public native void  openShapeJNI();
public native void  openEmbDBNI();
public native void  openSqliteJNI();
*/

extern "C"	jint
	Java_com_visualgdb_example_mapviewer_MapFrameActivity_openShapeJNI( JNIEnv* env,
	jobject thiz )
{
	CommonLib::CString str;
	str = "Test";
	int value = 0;
	return 0; 
}

extern "C"	jint
	Java_com_visualgdb_example_mapviewer_MapFrameActivity_openEmbDBNI( JNIEnv* env,	jobject thiz )
{

	CommonLib::CString str;
	str = "Test";
	int value = 0;
	return 0;
}


extern "C"	jint
	Java_com_visualgdb_example_mapviewer_MapFrameActivity_openSqliteJNI( JNIEnv* env,
	jobject thiz )
{
	CommonLib::CString str;
	str = "Test";
	int value = 0;
	return 0;
}