package app.com.mapviewer;

/**
 * Created by Slava on 10.01.2016.
 */
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.graphics.Rect;
import android.view.View;
import java.io.File;
import java.util.ArrayList;

public class MapView extends SurfaceView implements SurfaceHolder.Callback
{
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

    @Override
    protected void onDraw(Canvas canvas)
    {
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
