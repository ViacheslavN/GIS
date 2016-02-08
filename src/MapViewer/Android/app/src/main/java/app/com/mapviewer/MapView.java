package app.com.mapviewer;

/**
 * Created by Slava on 10.01.2016.
 */
import android.graphics.Point;
import android.graphics.Rect;
import android.os.Message;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.content.Context;
import android.graphics.Bitmap;
import android.view.Display;
import android.graphics.Canvas;
import android.util.*;
import android.os.Handler;

import app.com.mapviewer.nativeApi.DisplayTransformation;
import app.com.mapviewer.nativeApi.Map;
import app.com.mapviewer.nativeApi.MapDrawer;


public class MapView extends SurfaceView implements SurfaceHolder.Callback
{
    public Bitmap mapBitmap = null;
    private MapDrawer mapDrawer = null;
    private MapFrameActivity mapFrame = null;
    private String mapName;
    private Map.extent_t curExtent;
    private Handler handler = new Handler() {
        public void  handleMessage(Message msg) {
            if(msg.what == 1)
            {
                if(mapBitmap != null)
                {
                    mapDrawer.copy(mapBitmap, new Point(msg.arg1, msg.arg2), (Rect)msg.obj);
                }
                //invalidate();
    		/* Canvas canvas = surfaceHolder_.lockCanvas();
    		 MapView.this.onDraw(canvas);
    		 surfaceHolder_.unlockCanvasAndPost(canvas); */
            }
        }
    };
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
        mapDrawer = new MapDrawer(this);
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
    public synchronized void update(Point outpt, Rect outRect, boolean bForce)
    {
        if(!bForce){
            Message msg = new Message();
            msg.what = 1;
            if(outpt != null){
                msg.arg1 = outpt.x;
                msg.arg2 = outpt.y;
            }
            else{
                msg.arg1 = 0;
                msg.arg2 = 0;
            }
            msg.obj = outRect;
            handler.sendMessage(msg);
        }
        else{
            mapDrawer.copy(mapBitmap, outpt, outRect);
            //invalidate();
            onRedraw();
        }


    }
    public void openMap(String patch)
    {
        WindowManager w = mapFrame.getWindowManager();

        Display d = w.getDefaultDisplay();
        int width = this.getWidth();
        int height = this.getHeight();

        DisplayMetrics outMetrics = new DisplayMetrics ();
        d.getMetrics(outMetrics);

        if(mapDrawer.openMap(patch, width, height, outMetrics.densityDpi))
        {
            Map map = mapDrawer.getMap();
            DisplayTransformation tr = mapDrawer.getTransformation();
          //  ShowActiveElement();

           // search_handler_.onOpenMap(map);

            curExtent  = map.getFullMapExtent();
            tr.setMapVisibleRect(curExtent);
            mapName = map.getMapName();
            if(mapBitmap == null)
            {
                mapBitmap =  Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
                mapBitmap.eraseColor(0xffffffff);
            }
            // drawer_.DrawMap(mBitmap);
            mapDrawer.redrawMap(true, null);
            // invalidate();
            // onRedraw();
        }
    }
    public void onRedraw(){
	/* Canvas canvas = surfaceHolder_.lockCanvas();
	 MapView.this.onDraw(canvas);
	 surfaceHolder_.unlockCanvasAndPost(canvas); */
    }
}
