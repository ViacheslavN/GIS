package app.com.mapviewer.nativeApi;

/**
 * Created by Slava on 07.02.2016.
 */
public class Layer extends CObject {

    String name_ = null;
    public Layer(int handle) {
        super(handle);
        // TODO Auto-generated constructor stub
    }

    @Override
    protected void DeleteNativeHandle(int handle) {
        // TODO Auto-generated method stub

    }


    public String getName()
    {
        if(name_ == null){
            name_ = getLayerNameN(handle_);
        }
        return name_;
    }
    public boolean getVisible(){
        return getVisibleN(this.getHandle());
    }
    public void setVisible(boolean bVisible){
        setVisibleN(this.getHandle(), bVisible);
    }
    public  boolean isActiveOnScale(double scale){
        return isActiveOnScaleN(this.getHandle(), scale);
    }
    //////////////////////////////////native////////////////////////////////////////////////
    private  native String  getLayerNameN(int ptr);
    private  native boolean  getVisibleN(int ptr);
    private  native void  setVisibleN(int ptr, boolean bVisible);
    private  native boolean isActiveOnScaleN(int ptr, double scale);
}
