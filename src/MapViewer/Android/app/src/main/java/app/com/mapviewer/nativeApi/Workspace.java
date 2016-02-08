package app.com.mapviewer.nativeApi;


public class Workspace extends CObject {

    public Workspace(int handle) {
        super(handle);
        // TODO Auto-generated constructor stub
    }

    @Override
    protected void DeleteNativeHandle(int handle) {
        // TODO Auto-generated method stub

    }
    public static Workspace getGlobalWorkspace(){
        int ptr = getGlobalWorkspaceN();
        if(ptr != 0)
            return new Workspace(ptr);
        return null;
    }
    //////////////////////naitve///////////////////////////////
    private native static int getGlobalWorkspaceN();
}

