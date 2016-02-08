package app.com.mapviewer.nativeApi;

import android.graphics.Bitmap;

public class FeatureLayer extends Layer {
    private FeatureClass featureclass_ = null;
    private LegengInfo legendinfo_ = null;
    public FeatureLayer(int handle) {
        super(handle);
        // TODO Auto-generated constructor stub
    }

    @Override
    protected void DeleteNativeHandle(int handle) {
        // TODO Auto-generated method stub

    }
    public FeatureClass GetFeatureClass(){
        if(featureclass_ == null){
            int handle = getFeatureClassN(this.getHandle());
            if(handle != 0)
                featureclass_ = new FeatureClass(handle);
        }
        return featureclass_;
    }

    public LegengInfo getLegendInfo(){
        if(legendinfo_ == null){
            int ptr = getLegendInfoN(this.getHandle());
            if(ptr != 0){
                legendinfo_ = new LegengInfo(ptr);
            }
        }
        return legendinfo_;

    }

    public Geometry getGeometryById(int oid, DisplayTransformation tr){
        return getGeometryByIdN(this.getHandle(), tr.getHandle(), oid);
    }
    ///////////////////////////////////////////native////////////////////////////////////
    private native int getFeatureClassN(int handle);
    private native int getLegendInfoN(int handle);
    private native Geometry getGeometryByIdN(int handle, int tr_handle, int oid);

}

