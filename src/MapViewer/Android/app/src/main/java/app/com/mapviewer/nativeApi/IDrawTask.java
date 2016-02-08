package app.com.mapviewer.nativeApi;


public interface IDrawTask {

    public static enum TypeTask
    {
        PAN_TASK,  MAP_TASK;
    };

    public void draw();
    public void cancelDraw(boolean bWait);
    public boolean isDrawind();
    public TypeTask getType();
}
