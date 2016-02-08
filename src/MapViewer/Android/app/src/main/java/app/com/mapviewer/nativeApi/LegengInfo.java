package app.com.mapviewer.nativeApi;

import android.graphics.Bitmap;

public class LegengInfo extends CObject {

    public LegengInfo(int handle) {
        super(handle);
        // TODO Auto-generated constructor stub
    }

    @Override
    protected void DeleteNativeHandle(int handle) {
        // TODO Auto-generated method stub

    }

    public int     GetItemCount(){
        return getItemCountN(this.getHandle());
    }
    public String  GetItemLabel(int index){
        return getItemLabelN(this.getHandle(), index);
    }
    public void DrawSymbol(int symbol, double dpi,  Bitmap bitmap){
        drawSymboltN(this.getHandle(), symbol, dpi, bitmap);
    }
    //////////////////////////////////////native/////////////////////////////////////////////
    private native int getItemCountN(int handle);
    private native String getItemLabelN(int handle, int index);
    private native void drawSymboltN(int handle, int symbol, double dpi, Bitmap bitmap);
}