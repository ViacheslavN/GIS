package app.com.mapviewer.native_api;

/**
 * Created by Slava on 10.01.2016.
 */
public abstract class CObject {
    protected int handle_ = 0;
    private boolean delete_ = false;

    public CObject(int handle){
        handle_ = handle;
    }
    public CObject(int handle, boolean bDel){
        handle_ = handle;
        delete_ = bDel;
    }

    public int getHandle(){
        return handle_;
    }

    public boolean isNull(){
        return handle_ == 0;
    }

    public void Dispose()
    {
        try
        {
            if (handle_ != 0)
                if (delete_)
                {
                    DeleteNativeHandle(handle_);
                }
        }
        finally
        {
            handle_ = 0;
        }
    }

    public boolean equals(CObject obj){
        if(obj == this)
            return true;
        if(obj == null)
            return false;
        if( obj instanceof CObject)
        {
            CObject map_obj = (CObject)obj;
            return this.getHandle() == map_obj.getHandle();
        }
        return false;
    }

    protected void finalize(){
        Dispose();

    }
    protected abstract void DeleteNativeHandle(int handle);

    static {
        try
        {
            System.loadLibrary("stlport_shared");
        }
        catch(UnsatisfiedLinkError ex)
        {
            String message = ex.getMessage();
            ex.printStackTrace();
        }
        try
        {
            System.loadLibrary("GisLibrary");
        }
        catch(UnsatisfiedLinkError ex)
        {
            String message = ex.getMessage();
            ex.printStackTrace();
        }
    }

}
