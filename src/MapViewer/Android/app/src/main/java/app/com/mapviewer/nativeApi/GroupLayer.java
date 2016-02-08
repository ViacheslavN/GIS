package app.com.mapviewer.nativeApi;

public class GroupLayer extends Layer {

    Layers layers_;
    public GroupLayer(int handle) {
        super(handle);
        // TODO Auto-generated constructor stub
    }
    @Override
    protected void DeleteNativeHandle(int handle) {
        // TODO Auto-generated method stub

    }

    public Layers GetChildren(){
        if(layers_ == null){
            layers_ = new Layers(getChildrenN(this.getHandle()));
        }
        return layers_;
    }
    ////////////////////////native///////////////////////////////
    private native int getChildrenN(int handle);
}
