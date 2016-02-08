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
 */
package com.visualgdb.example.mapviewer;

import android.app.Activity;
import android.view.View;
import android.widget.Button;
import android.os.Bundle;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ArrayAdapter;
import android.widget.TextView;
public class MapFrameActivity extends Activity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

		

        /* Create a Button and set its content.
         * the text is retrieved by calling a native
         * function.
         */


		 LinearLayout linLayout = new LinearLayout(this);

		 

		final Button  OpenShapeBtn = new Button(this);
		OpenShapeBtn.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				 openShapeJNI();
			}
		});
        OpenShapeBtn.setText( "OpenShape" );
		linLayout.addView(OpenShapeBtn);

		
		final Button  OpenEmbDBBtn = new Button(this);
		OpenEmbDBBtn.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				 openEmbDBNI();
			}
		});
        OpenEmbDBBtn.setText( "OpenEmbDB" );
		linLayout.addView(OpenEmbDBBtn);
		


		final Button  OpenSqlLite = new Button(this);
		OpenSqlLite.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				 openSqliteJNI();
			}
		});
        OpenSqlLite.setText( "OpenSqlite" );
		linLayout.addView(OpenSqlLite);
		
		setContentView(linLayout);
		


       /* final Button  button = new Button(this);
		button.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				button.setText( stringFromJNI());
			}
		});
        button.setText( stringFromJNI() );
        setContentView(button);*/
    }

    /* A native method that is implemented by the
     * 'GisLibrary' native library, which is packaged
     * with this application.
     */
    public native String  stringFromJNI();
	public native int  openShapeJNI();
	public native int  openEmbDBNI();
	public native int  openSqliteJNI();
    /* this is used to load the 'GisLibrary' library on application
     * startup. The library has already been unpacked into
     * /data/data/com.visualgdb.example.mapviewer/lib/GisLibrary.so at
     * installation time by the package manager.
     */
    static {
        System.loadLibrary("GisLibrary");
    }
}
