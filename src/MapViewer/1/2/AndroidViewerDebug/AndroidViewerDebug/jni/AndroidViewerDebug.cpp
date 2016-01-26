/*
 * Created by VisualGDB. Based on hello-jni example.

 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include <stdio.h>
#include "MapDrawer.h"
#include "CommonLibrary\String.h"

int s_ButtonPressCounter = 0;
extern "C"
jstring
Java_app_com_mapviewer_AndroidViewerDebug_AndroidViewerDebug_stringFromJNI( JNIEnv* env,
                                                  jobject thiz )
{
	char szBuf[512];
	sprintf(szBuf, "You dddd have pressed this huge button %d times", s_ButtonPressCounter++);
  
	CommonLib::CString sStr;
	sStr = szBuf;
	jstring str = (*env).NewStringUTF(sStr.cstr());
	
	CMapDrawer *pDrawer = new CMapDrawer(96.2);
	pDrawer->SetSize(10, 100);
	delete pDrawer;
	//jstring str = (*env).NewStringUTF(szBuf);
	return str;
}
