package app.com.mapviewer.nativeApi;
import  app.com.mapviewer.MapView;
import android.graphics.Bitmap;
import android.graphics.Point;
import android.graphics.Rect;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.Message;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.WindowManager;

public class MapDrawer extends CObject
{
    private  Map map_ = null;
    private DisplayTransformation fake_trans_ = null;
    private DisplayTransformation map_trans_ = null;
    private DrawindThread drawing_thread_ = null;
    private MapDrawTask  draw_map_task_ = null;
    private PanTask     pan_task_ = null;
    //private MapDrawTask  draw_lable_task_ = null;
    private UpdateTimer  timer_ = null;
    private MapView view_ = null;
    private int flags_;

    private final int DRAW_MAP = 1;
    private final int PAN_STATE  = 2;
    private final int STOPPING_PAN = 4;
    private final int FINSHED_PAN_STATE = 8;
    private final int PAN_AFTER_MAP = 16;
    private final int FINSHED_DRAW_MAP  = 32;
    private final int STOP_MAP_DRAW  = 64;
    private final int DRAW_GEOMETRY = 128;
    private final int DRAW_LABLE = 256;

    private boolean   bIdentify_ = true;

    private Point begin_pan_;

    private int width_;
    private int height_;
    int tolerance_ = 0;
    boolean bCopy = true;


    private final int BEGIN_TASK_DRAW  = 1;
    private final int END_TASK_DRAW = 2;







    class UpdateTimer extends CountDownTimer
    {
        boolean bWork_ = false;
        public UpdateTimer(long millisInFuture, long countDownInterval)
        {
            super(millisInFuture, countDownInterval);
            // TODO Auto-generated constructor stub
        }

        @Override
        public void onFinish()
        {


        }

        @Override
        public void onTick(long millisUntilFinished)
        {
            OnUpdateTimer();
        }

        public void onStart(){
            bWork_ = true;
            this.start();
        }
        public void onStop(){
            bWork_ = false;
            this.cancel();
        }
        public boolean IsWork(){
            return bWork_;
        }
    };

    public MapDrawer(MapView view) {
        super(createMapDrawerN(), true);
        // TODO Auto-generated constructor stub

        drawing_thread_ = new DrawindThread();
        drawing_thread_.start();

        draw_map_task_ = new MapDrawTask(createMapTaskN(this.getHandle()), this, IDrawTask.TypeTask.MAP_TASK);
        pan_task_  = new PanTask(createPanTaskN(this.getHandle()), this, IDrawTask.TypeTask.PAN_TASK);
        //	draw_lable_task_ = new MapDrawTask(createLableTaskN(this.getHandle()), this, IDrawTask.TypeTask.LABLE_TASK);
        view_ = view;
        SetFlag(FINSHED_DRAW_MAP);
        begin_pan_ = new Point();

    }

    @Override
    protected void DeleteNativeHandle(int handle) {
        deleteMapDrawerN(handle);

    }

    public void redrawMap( boolean erase, Bitmap  bitmap){
        if(map_ == null)
        {
            return;
        }
        stopDraw();

        draw_map_task_.setDraw();
        redrawMapN(this.getHandle(), erase, bitmap);
        SetFlag(DRAW_MAP|DRAW_GEOMETRY);
        drawing_thread_.setTask(draw_map_task_, true);
        timer_ = new  UpdateTimer(500000, 1000);
        timer_.onStart();
    }

    public void stopDraw(){
        if(timer_ != null)
            timer_.onStop();
        drawing_thread_.stopDraw(true, true);
    }

    public boolean openMap(String patch,  int width, int height, int dpi)
    {
        width_ = width;
        height_ = height;
        int handle = openMapN(this.getHandle(), patch, width, height, dpi);
        //int handle = openAGSMapN(this.getHandle(),  width, height, dpi);
        if(handle != 0)
        {

            map_ = new Map(handle);
            fake_trans_ = new DisplayTransformation(createFakeTransformationN(this.getHandle()));
            map_trans_ = new DisplayTransformation(createMapTransformationN(this.getHandle()));
            tolerance_ = (int)DisplayUtils.SymbolSizeToDeviceSize(map_trans_, 2.0, false);
        }
        return handle != 0;
    }


    public DisplayTransformation getTransformation(){
        return fake_trans_;
    }
    public Map getMap(){
        return map_;
    }


    public void  copy(Bitmap  bitmap, Point pt, Rect rect)
    {

        if(!bCopy)
            return;

        if(map_ == null)
        {
            bitmap.eraseColor(0xffffffff);
        }
        else
        {
            Point outPt;
            Rect outRect;

            if(pt != null)
            {
                outPt = pt;
            }
            else
            {
                outPt = new Point(0, 0);
            }
            if(rect != null){
                outRect = rect;
            }
            else{
                outRect = new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight());
            }
            if(IsFlag(PAN_STATE)||IsFlag(STOPPING_PAN))
            {
                if(!IsFlag(FINSHED_DRAW_MAP))
                {
                    // pGraphics->Erase(Display::Color(255 , 255 , 255));

                    //Display::GPoint pt(0, 0);
                    bitmap.eraseColor(0xffffffff);
                    int pan_x = pan_task_.GetPanFullOffsetX();
                    int pan_y = pan_task_.GetPanFullOffsetY();
                    Rect pan_rect = new Rect(- pan_x, - pan_y, (width_ - pan_x), (height_- pan_y));
                    Rect wnd_rect  = new Rect( 0, 0, width_, height_);

                    if(Rect.intersects(wnd_rect, pan_rect))
                    {
                        //pGraphics->Copy(/*pMapGraphics_*/pOutGraphics_.get(), pt, rect, false);
                        copyMapGraphicsN(this.getHandle(), 0, 0, pan_rect.left, pan_rect.top, pan_rect.right, pan_rect.bottom, bitmap);
                    }
                }
                else
                {
                    //pGraphics->Copy(pOutGraphics_.get(), OutPoint, OutRect, false);
                    copyOutGraphicsN(this.getHandle(), outPt.x, outPt.y, outRect.left, outRect.top, outRect.right, outRect.bottom, bitmap);
                }
            }
            else
            {
                if(IsFlag(DRAW_GEOMETRY))
                {
                    copyMapGraphicsN(this.getHandle(), outPt.x, outPt.y, outRect.left, outRect.top, outRect.right, outRect.bottom, bitmap);
                }
                else
                {
                    copyOutGraphicsN(this.getHandle(), outPt.x, outPt.y, outRect.left, outRect.top, outRect.right, outRect.bottom, bitmap);
                }

            }
            //copyN(this.getHandle(), bitmap);
        }

    }

    public void copyMapGraphicsInOutGraphics(int x, int y, int left, int top, int right, int bottom){
        copyMapGraphicsInOutGraphicsN(this.getHandle(), x, y, left, top, right, bottom);
    }
    private synchronized  void AddFlags(int add_flag, int remove_flag){
        flags_ |= add_flag;
        flags_ &= ~remove_flag;
    }

    private synchronized void SetFlag(int flag){
        flags_ = flag;
    }
    private synchronized boolean IsFlag(int flag){
        return (flags_ & flag) == flag;
    }
    public void  OnUpdateTimer(){
        view_.update(null, null, false);
    }

    public void drawRotateMap(Bitmap  bitmap, double angle){
        drawRotateMapN(this.getHandle(), bitmap, angle);
    }
    public synchronized void onFinishedTask(IDrawTask task, boolean breaktask){

        Message msg = new Message();
        msg.what = END_TASK_DRAW;
        msg.obj = task;
        msg.arg1 = breaktask ? 1 : 0;
        handler.sendMessage(msg);
    }

    public  void onFinishedTaskMessage(IDrawTask task, boolean breaktask){
        if(task.getType() == IDrawTask.TypeTask.MAP_TASK)
        {

            if(IsFlag(PAN_AFTER_MAP)){
                //AddFlags(STOPPING_PAN, PAN_AFTER_MAP);
                SetFlag(STOPPING_PAN|FINSHED_DRAW_MAP);
                //pan_task_.copyMapGraphics(0, 0);
                copyMapGraphicsInOutGraphics(0, 0, 0, 0, width_, height_);
                pan_task_.setDraw();
                drawing_thread_.setTask(pan_task_, true);
            }
            else  if(IsFlag(PAN_STATE)){
                SetFlag(PAN_STATE|FINSHED_DRAW_MAP);
                //pan_task_.copyMapGraphics(pan_task_.GetPanOffsetX(), pan_task_.GetPanFullOffsetY());
                int panx = pan_task_.GetPanFullOffsetX();
                int pany = pan_task_.GetPanFullOffsetY();
                copyMapGraphicsInOutGraphics(-panx, -pany, 0, 0, width_ - panx, height_ - pany);
            }
            else
            {
                if(breaktask)
                {
                    SetFlag(FINSHED_DRAW_MAP|STOP_MAP_DRAW);
                }
                else
                {
                    SetFlag(FINSHED_DRAW_MAP);
                }

                timer_.onStop();
                view_.update(null, null, true);

            }
        }
        else if(task.getType() == IDrawTask.TypeTask.PAN_TASK)
        {
            if(!breaktask){
                timer_.onStop();
                view_.update(null, null, true);
                int pan_x = pan_task_.GetPanFullOffsetX();
                int pan_y = pan_task_.GetPanFullOffsetY();
                //pan_task_.Erase();
                Rect rect = new Rect(pan_x, pan_y, (width_ + pan_x), (height_ + pan_y));
                SetNewPanRect(rect);
                SetFlag(DRAW_MAP|DRAW_LABLE);
                draw_map_task_.setLableDraw();
                drawing_thread_.setTask(draw_map_task_, true);
            }
            else{
                int i = 0;
                ++i;
            }

        }
    }

    public synchronized void onBeginTaskDraw(IDrawTask task){

        Message msg = new Message();
        msg.what = BEGIN_TASK_DRAW;
        msg.obj = task;
        handler.sendMessage(msg);
    }
    public synchronized void onBeginTaskDrawMessage(IDrawTask task){
        if(task.getType() == IDrawTask.TypeTask.PAN_TASK)
        {
            timer_.onStart();
        }
    }
    public int getWidth(){
        return width_;
    }
    public int getHeight(){
        return height_;
    }
    public void onFinishedGeometryDraw(IDrawTask task, boolean breaktask){
        if(task.getType() == IDrawTask.TypeTask.MAP_TASK)
        {
            if(!breaktask)
                AddFlags(DRAW_LABLE, DRAW_GEOMETRY);
        }
    }

    public void	onStartPan(int x, int y){
        if(timer_ != null){
            timer_.onStop();
        }


        bIdentify_ = true;
        begin_pan_.x = x;
        begin_pan_.y = y;

        AddFlags(PAN_STATE, 0);
        if(IsFlag(DRAW_MAP))
        {
            AddFlags(PAN_AFTER_MAP, 0);
        }
        pan_task_.onStartPan(x, y, IsFlag(FINSHED_DRAW_MAP));

    }
    public void	onMovePan(int x, int y){
        if(!IsFlag(PAN_STATE))
            return;

        int off_x = begin_pan_.x - x;
        int off_y = begin_pan_.y -y;

        if(off_x == 0 && off_y == 0)
        {
            return;
        }

        pan_task_.onMovePan(off_x, off_y, IsFlag(FINSHED_DRAW_MAP));
        int pan_x = pan_task_.GetPanFullOffsetX();
        int pan_y = pan_task_.GetPanFullOffsetY();
        if(bIdentify_)
        {
            if(Math.abs(pan_task_.GetPanFullOffsetX()) > tolerance_ || Math.abs(pan_task_.GetPanFullOffsetY()) > tolerance_ )
            {
                bIdentify_ = false;
            }
        }


        double angle = map_trans_.getAngle();
        Rect rect = new  Rect(pan_x, pan_y, (width_ + pan_x), (height_ + pan_y));
        Map.extent_t bbox = new Map.extent_t();
        map_trans_.deviceToMap(rect , bbox);
        if(angle != 0.0)
        {
            Map.GisXYPoint centerPt =  new  Map.GisXYPoint((bbox.maxy_ + bbox.minx_)/2, (bbox.maxy_ + bbox.miny_)/2);
            fake_trans_.setMapPos(centerPt, map_trans_.getScale());
        }
        else
        {
            fake_trans_.setMapVisibleRect(bbox);
        }

        Point pan_pt = new Point(0, 0);
        Rect pan_rect = new Rect(-pan_x, -pan_y, (width_ - pan_x), (height_- pan_y));
        //OnInvalidate(&pan_pt, &pan_rect, true);
        view_.update(pan_pt, pan_rect, true);
        begin_pan_.x = x;
        begin_pan_.y = y;

    }

    private void SetNewPanRect(Rect rect)
    {
        Map.extent_t bbox = new  Map.extent_t();
        map_trans_.deviceToMap(rect , bbox);
        // transformation_->SetMapVisibleRect(bbox);
        double angle = map_trans_.getAngle();
        if(angle != 0.0)
        {
            double scale = map_trans_.getAngle();
            Map.GisXYPoint centerPt = new  Map.GisXYPoint((bbox.maxx_ + bbox.minx_)/2, (bbox.maxy_ + bbox.miny_)/2);
            map_trans_.setMapPos(centerPt, scale);
            fake_trans_.setMapPos(centerPt, scale);
        }
        else
        {
            map_trans_.setMapVisibleRect(bbox);
            fake_trans_.setMapVisibleRect(bbox);
        }
    }

    public boolean	onStopPan(int x, int y){
        pan_task_.onStopPan(x, y, IsFlag(FINSHED_DRAW_MAP) && !bIdentify_);
        if(IsFlag(FINSHED_DRAW_MAP) && !pan_task_.isDrawind())
        {
            if(!bIdentify_)
            {
                //bCopy = false;
                pan_task_.setDraw();
                drawing_thread_.setTask(pan_task_, true);
                timer_ = new  UpdateTimer(500000, 1000);
                //timer_.start();

            }
            else
            {
                fake_trans_.setMapPos( map_trans_.getMapPos() ,  map_trans_.getScale());
                fake_trans_.setAngle(map_trans_.getAngle());
		    /*  pOutGraphics_->Copy(pLableGraphics_.get(), Display::GPoint(0, 0),  Display::GRect(0, 0, (Display::GUnits)width_, (Display::GUnits)height_), false);*/

                AddFlags(0, PAN_STATE);
                view_.update(null, null, true);
                return false;
            }
        }

        AddFlags(STOPPING_PAN, PAN_STATE);
        return true;
    }

    private Handler handler = new Handler() {
        public void  handleMessage(Message msg) {
            switch(msg.what){
                case END_TASK_DRAW:
                    onFinishedTaskMessage((IDrawTask) msg.obj, msg.arg1 == 1);
                    break;
                case BEGIN_TASK_DRAW:
                    onBeginTaskDrawMessage((IDrawTask) msg.obj);
                    break;
            }

        }
    };
    public boolean isFinishDraw(){
        return flags_ == FINSHED_DRAW_MAP;
    }

    ////////////////////////native//////////////////////////////
    private native static int createMapDrawerN();
    private native void deleteMapDrawerN(int handle);
    private native int createFakeTransformationN(int handle);
    private native int createMapTransformationN(int handle);
    private native int createMapTaskN(int handle);
    private native int createLableTaskN(int handle);
    private native int createPanTaskN(int handle);

    private native int openMapN(int handle, String patch,  int width, int height, int dpi);
    private native void closeMapN(int handle);
    private native int openAGSMapN(int handle, int width, int height, int dpi);

    private native void drawRotateMapN(int ptr, Bitmap  bitmap, double angle);
    private native void redrawMapN(int handle, boolean bErase,  Bitmap  bitmap);
    //private native void copyN(int handle,  Bitmap  bitmap);


    private native void copyMapGraphicsN(int handle, int x, int y, int left, int top, int right, int bottom, Bitmap  bitmap);
    private native void copyOutGraphicsN(int handle, int x, int y, int left, int top, int right, int bottom, Bitmap  bitmap);
    private native void copyMapGraphicsInOutGraphicsN(int handle, int x, int y, int left, int top, int right, int bottom);
}
