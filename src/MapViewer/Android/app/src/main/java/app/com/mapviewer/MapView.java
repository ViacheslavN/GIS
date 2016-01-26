package app.com.mapviewer;

/**
 * Created by Slava on 10.01.2016.
 */
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.content.Context;
import android.graphics.Bitmap;
import android.view.Display;
import android.graphics.Canvas;
import android.util.*;

import app.com.mapviewer.nativeApi.MapDrawer;


public class MapView extends SurfaceView implements SurfaceHolder.Callback
{
    public Bitmap mapBitmap = null;
    private MapDrawer mapDrawer = null;
    private MapFrameActivity mapFrame = null;

    public MapView(Context context) {
        super(context);
        init(false, 0, 0);

    }
    public MapView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        init(false, 0, 0);

    }
    public MapView(Context context, AttributeSet attrs, int defStyle) {
        super(context/*, attrs, defStyle*/);
        init(false, 0, 0);
    }
    public MapView(Context context, boolean translucent, int depth, int stencil) {
        super(context);
        init(translucent, depth, stencil);
    }
    private void init(boolean translucent, int depth, int stencil)
    {

    }
    public  void SetMapFrame(MapFrameActivity mf)
    {
        mapFrame = mf;
        WindowManager w = mapFrame.getWindowManager();

        Display d = w.getDefaultDisplay();
        int width = this.getWidth();
        int height = this.getHeight();

        DisplayMetrics outMetrics = new DisplayMetrics ();
        d.getMetrics(outMetrics);

        mapDrawer = new MapDrawer(outMetrics.densityDpi);
    }
    @Override
    protected void onDraw(Canvas canvas)
    {
        if(mapBitmap != null)
        {
            canvas.drawBitmap(mapBitmap, 0, 0, null);
        }
        else
        {
            canvas.drawColor(0xffffffff);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {


    }
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {


    }
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {


    }
}
