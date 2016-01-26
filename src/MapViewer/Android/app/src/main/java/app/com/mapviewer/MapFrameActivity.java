package app.com.mapviewer;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class MapFrameActivity extends AppCompatActivity {

    private MapView mapView = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map_frame);
        mapView = (MapView) findViewById(R.id.MapViewID);
        mapView.SetMapFrame(this);

        MapViewerApplication app =   (MapViewerApplication)this.getApplication();
        app.setMapFrame(this);
    }
}
