package app.com.mapviewer.nativeApi;

/**
 * Created by Slava on 10.01.2016.
 */
public class Map extends CObject {
    public static class extent_t {
        public double minx_;
        public double miny_;
        public double maxx_;
        public double maxy_;

        public extent_t(){

        }
        public extent_t(double minx, double miny, double maxx, double maxy){
            minx_ = minx;
            miny_ = miny;
            maxx_ = maxx;
            maxy_ = maxy;
        }
    };

    public static class GisXYPoint{
        public double x_;
        public double y_;
        public GisXYPoint(){

        }
        public GisXYPoint(double x, double y){
            x_ = x;
            y_ = y;
        }
    }

    public Map(int handle)
    {
        super(handle);
    }
    @Override
    protected void DeleteNativeHandle(int handle) {
    }

    public extent_t  getFullMapExtent()
    {
        extent_t ext = new extent_t();
        double me[] = new double[4];
        getFullMapExtentN (handle_, me);

        ext.minx_ = me[0];
        ext.miny_ = me[1];
        ext.maxx_ = me[2];
        ext.maxy_ = me[3];
        return ext;
    }
    public String getMapName()
    {
        return getMapNameN(handle_);
    }


    //////////////////////////Native//////////////////////////////////

    private  native int  getFullMapExtentN(int ptr, double arr[]);
    private  native String  getMapNameN(int ptr);
    private  native int  getLayersN(int ptr);
    private  native int  getMapWorkspaceN(int ptr);
    private  native int  getPushPinWorkspaceN(int ptr);
}

