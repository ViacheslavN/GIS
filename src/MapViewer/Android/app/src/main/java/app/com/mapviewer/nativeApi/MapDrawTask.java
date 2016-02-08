package app.com.mapviewer.nativeApi;

public class MapDrawTask extends CObject implements IDrawTask{

    private MapDrawer drawer_;
    private TypeTask type_;
    private boolean continue_;
    private boolean drawing_;
    private boolean lableonly_ = false;

    public MapDrawTask(int handle, MapDrawer drawer, TypeTask type) {
        super(handle);
        drawer_ = drawer;
        type_ = type;
        continue_ = true;
        drawing_ = false;
    }

    @Override
    protected void DeleteNativeHandle(int handle) {
        // TODO Auto-generated method stub

    }
    //IDrawTask
    @Override
    public void draw() {
        drawing_ = true;
        //drawN(this.getHandle());
        if(!lableonly_)
        {
            drawGeometryN(this.getHandle());
            drawer_.onFinishedGeometryDraw(this, !continue_);
        }
        if(continue_)
        {
            setLableDrawN(this.getHandle(), !lableonly_);
            drawLableN(this.getHandle());
        }
        drawing_ = false;
        drawer_.onFinishedTask(this, !continue_);

    }

    @Override
    public void cancelDraw(boolean bWait) {
        continue_ = false;
        cancelDrawN(this.getHandle(), bWait);
        // TODO Auto-generated method stub

    }

    @Override
    public boolean isDrawind() {
        return drawing_;
    }
    public void setDraw() {
        continue_ = true;
        lableonly_ = false;
        setDrawN(this.getHandle());
    }
    @Override
    public TypeTask getType() {
        // TODO Auto-generated method stub
        return type_;
    }
    public void setLableDraw(){
        continue_ = true;
        lableonly_ = true;
        //setLableDrawN(this.getHandle(), false);
    }
    ////////////////////////native//////////////////////////////
    //private native void drawN(int handle);
    private native void setDrawN(int handle);
    private native void cancelDrawN(int handle, boolean bWait);

    private native void drawGeometryN(int handle);
    private native void drawLableN(int handle);
    private native void setLableDrawN(int handle, boolean bUseCache);


}
