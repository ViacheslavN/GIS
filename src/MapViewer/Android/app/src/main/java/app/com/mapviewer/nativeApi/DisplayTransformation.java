package app.com.mapviewer.nativeApi;

import android.graphics.Rect;


public class DisplayTransformation extends CObject {

    public static final int POINT_TYPE = 1;
    public static final int RECT_TYPE = 2;



    public DisplayTransformation(int handle) {
        super(handle);
        // TODO Auto-generated constructor stub
    }

    @Override
    protected void DeleteNativeHandle(int handle) {
        // TODO Auto-generated method stub

    }

    public Map.extent_t  getCurMapExtent()
    {

        Map.extent_t ext = new Map.extent_t();
        double me[] = new double[4];
        getCurMapExtentN (handle_, me);
        ext.minx_ = me[0];
        ext.miny_ = me[1];
        ext.maxx_ = me[2];
        ext.maxy_ = me[3];
        return ext;

    }
    public void setMapVisibleRect(Map.extent_t ext)
    {
        double me[] = new double[4];
        me[0] = ext.minx_;
        me[1] = ext.miny_;
        me[2] = ext.maxx_;
        me[3] = ext.maxy_;
        setMapVisibleRectN(this.getHandle(), me);
    }


    public double getAngle()
    {
        return getAngleMapN(this.getHandle());
    }
    public double getScale()
    {
        return getScaleN(this.getHandle());
    }

    public void setAngle(double angle)
    {
        setAngleMapN(this.getHandle(), angle);
    }

    public Rect getDeviceRect()
    {
        Rect devRect = new Rect();

        int rd[] = new int[4];
        getDeviceRectN(this.getHandle(), rd);
        devRect.left = rd[0];
        devRect.top = rd[1];
        devRect.right = rd[3];
        devRect.bottom = rd[4];
        return devRect;
    }

    public Map.GisXYPoint getMapPos(){
        Map.GisXYPoint xy = new Map.GisXYPoint();
        double xys[] = new double[2];
        getMapPosN(this.getHandle(), xys);
        xy.x_ = xys[0];
        xy.y_ = xys[1];
        return xy;
    }

    public void setMapPos(Map.GisXYPoint xy, double scale){

        double xys[] = new double[3];
        xys[0] = xy.x_;
        xys[1] = xy.y_;
        xys[2] = scale;
        setMapPosN(this.getHandle(), xys);
    }
    public int getUnits()
    {
        return getUnitsN(this.getHandle());
    }

    public void deviceToMap(Rect rect, Map.extent_t ext){
        double rd[] = new double[4];
        double me[] = new double[4];

        rd[0] = rect.left;
        rd[1] = rect.top;
        rd[2] = rect.right;
        rd[3] = rect.bottom;

        deviceToMapN(this.getHandle(), RECT_TYPE,  4, rd, me);

        ext.minx_ = me[0];
        ext.miny_ = me[1];
        ext.maxx_ = me[2];
        ext.maxy_ = me[3];

    }

    public void mapToDeviceN(Map.extent_t ext, Rect rect){
        double rd[] = new double[4];
        double me[] = new double[4];
        me[0] = ext.minx_;
        me[1] = ext.miny_;
        me[2] = ext.maxx_;
        me[3] = ext.maxy_;

        mapToDeviceN(this.getHandle(), RECT_TYPE,  4,  me, rd);

        rect.left = (int)rd[0];
        rect.top =   (int)rd[1];
        rect.right = (int)rd[2];
        rect.bottom = (int)rd[3];


    }

    public double deviceToMapMeasure( double deviceLen){
        return deviceToMapMeasureN(this.getHandle(), deviceLen);
    }

    public double mapToDeviceMeasure(double mapLen){
        return mapToDeviceMeasureN(this.getHandle(), mapLen);
    }

    public double getResolution(){
        return getResolutionN(this.getHandle());
    }

    /////////////////////////////////native///////////////////////////////////////////////////////
    private  native void  getCurMapExtentN(int ptr, double arr[]);
    private native void setMapVisibleRectN(int ptr, double arr[]);
    private  native String  getMapNameN(int ptr);
    private  native double  getAngleMapN(int ptr);
    private  native double  getResolutionN(int ptr);
    private  native void  setAngleMapN(int ptr, double angle);
    private native double getScaleN(int ptr);
    private  native void  getMapPosN(int ptr, double arr[]);
    private  native void  setMapPosN(int ptr, double arr[]);
    private  native int  getUnitsN(int ptr);

    private  native void  getDeviceRectN(int ptr, int arr[]);
    private  native void  setDeviceRectN(int ptr, int arr[]);
    private native void deviceToMapN(int ptr, int type, int size, double dev_arr[], double map_arr[]);
    private native void mapToDeviceN(int ptr, int type, int size, double map_arr[],  double dev_arr[]);
    private native double deviceToMapMeasureN(int ptr, double deviceLen);
    private native double mapToDeviceMeasureN(int ptr, double mapLen);
}

