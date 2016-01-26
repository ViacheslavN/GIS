package app.com.mapviewer.nativeApi;

/**
 * Created by Slava on 26.01.2016.
 */
public class MapDrawer extends CObject
{

    public MapDrawer(int dpi) {

        super(createMapDrawerN(dpi), true);
    }
    @Override
    protected void DeleteNativeHandle(int handle) {
    }
    ////////////////////////native//////////////////////////////
    private native static int createMapDrawerN(int dpi);
}
