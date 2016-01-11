package app.com.mapviewer;

/**
 * Created by Slava on 10.01.2016.
 */
import android.app.Application;

public class MapViewerApplication extends Application
{
    private MapFrameActivity mapFrame_ = null;

    void setMapFrame(MapFrameActivity mapFrame){
        mapFrame_ = mapFrame;
    }
}
