package app.com.mapviewer.nativeApi;


public class DisplayUtils {


    static double  SymbolSizeToDeviceSize(DisplayTransformation trans, double sizeSymb, boolean scaleDependent ){
        return SymbolSizeToDeviceSizeN(trans.getHandle(), sizeSymb, scaleDependent);
    }
    ////////////////////////native/////////////////////////////////////////
    static private  native double  SymbolSizeToDeviceSizeN(int trans, double sizeSymb, boolean scaleDependent);

}
