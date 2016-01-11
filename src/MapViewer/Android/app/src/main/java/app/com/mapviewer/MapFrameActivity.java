package app.com.mapviewer;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class MapFrameActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map_frame);


        MapViewerApplication app =   (MapViewerApplication)this.getApplication();
        app.setMapFrame(this);
    }
}
