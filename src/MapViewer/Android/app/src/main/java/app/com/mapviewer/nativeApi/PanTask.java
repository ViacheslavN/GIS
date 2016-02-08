package app.com.mapviewer.nativeApi;

import android.graphics.Rect;

public class PanTask extends CObject implements IDrawTask {

    private MapDrawer drawer_;
    private TypeTask type_;
    private boolean continue_ = true;
    private boolean drawing_ = false;

    int pan_offset_x_ = 0;
    int pan_offset_y_ = 0;

    int full_pan_offset_x_ = 0;
    int full_pan_offset_y_ = 0;
    //int width_ = 0;
    //int height_ = 0;

    Rect hor_rect_;
    Rect vert_rect_;

    Rect map_hor_rect_;
    Rect map_vert_rect_;
    Rect wndRect_;
    boolean new_task_ = false;
    boolean bPan_ = false;

    public PanTask(int handle, MapDrawer drawer, TypeTask type) {
        super(handle);
        drawer_ = drawer;
        type_ = type;
        //width_ = drawer_.getWidth();
        //height_ = drawer_.getHeight();

        hor_rect_ = new Rect();
        vert_rect_ = new Rect();

        map_hor_rect_ = new Rect();
        map_vert_rect_ = new Rect();
        wndRect_ = new Rect();
    }

    void Erase() {
        pan_offset_x_ = 0;
        pan_offset_y_ = 0;
        full_pan_offset_x_ = 0;
        full_pan_offset_y_ = 0;
        hor_rect_.set(0, 0, 0, 0);
        vert_rect_.set(0, 0, 0, 0);
        wndRect_.set(0, 0, drawer_.getWidth(), drawer_.getHeight());
    }

    public void onStartPan(int x, int y, boolean bNew) {

        boolean bdraw = isDrawind();

        if(isDrawind()){
            drawer_.stopDraw();
        }
        //cancelDraw(true);
        if(bdraw){
            int i = 0;
            i++;
        }
        if (!new_task_ && bNew) {
            Erase();
        } else {
            //copyMapGraphicsN(this.getHandle(), 0, 0);
            drawer_.copyMapGraphicsInOutGraphics(0, 0, 0, 0, drawer_.getWidth(), drawer_.getHeight());
        }
        bPan_ = true;
    }



    public void onMovePan(int x, int y, boolean bDraw) {
        if (x == 0 && y == 0)
            return;
        pan_offset_x_ += x;
        pan_offset_y_ += y;

        full_pan_offset_x_ += x;
        full_pan_offset_y_ += y;

	/*	if (bDraw) {
			Rect PanRect = new Rect(full_pan_offset_x_, full_pan_offset_y_,
					width_ + full_pan_offset_x_, height_ + full_pan_offset_y_);

			if (wndRect_.height() == 0 || wndRect_.width() == 0) {
				wndRect_.set(0, 0, width_, height_);
			}
			boolean ret = difference_rect(wndRect_, PanRect, map_hor_rect_,
					map_vert_rect_);
			// assert(ret);

			if (!ret) {
				pan_offset_x_ = 0;
				pan_offset_y_ = 0;
				wndRect_ = PanRect;
				map_hor_rect_ = PanRect;
				map_vert_rect_.set(0, 0, 0, 0);
				vert_rect_.set(0, 0, 0, 0);
			}

			hor_rect_.left = map_hor_rect_.left - full_pan_offset_x_;
			hor_rect_.right = map_hor_rect_.right - full_pan_offset_x_;
			hor_rect_.top = map_hor_rect_.top - full_pan_offset_y_;
			hor_rect_.bottom = map_hor_rect_.bottom - full_pan_offset_y_;

			if (ret) {
				vert_rect_.left = map_vert_rect_.left - full_pan_offset_x_;
				vert_rect_.right = map_vert_rect_.right - full_pan_offset_x_;
				vert_rect_.top = map_vert_rect_.top - full_pan_offset_y_;
				vert_rect_.bottom = map_vert_rect_.bottom - full_pan_offset_y_;
			}
			new_task_ = true;
		}*/
    }

    public void onStopPan(int x, int y, boolean bDraw) {
        bPan_ = false;
        CalcRect();
        if (bDraw) {
            new_task_ = true;
        }
    }

    public void CalcRect(){
        Rect PanRect = new Rect(full_pan_offset_x_, full_pan_offset_y_,
                drawer_.getWidth() + full_pan_offset_x_, drawer_.getHeight() + full_pan_offset_y_);

        if (wndRect_.height() == 0 || wndRect_.width() == 0) {
            wndRect_.set(0, 0, drawer_.getWidth(), drawer_.getHeight());
        }
        boolean ret = difference_rect(wndRect_, PanRect, map_hor_rect_,
                map_vert_rect_);
        // assert(ret);

        if (!ret) {
            pan_offset_x_ = 0;
            pan_offset_y_ = 0;
            wndRect_ = PanRect;
            map_hor_rect_ = PanRect;
            map_vert_rect_.set(0, 0, 0, 0);
            vert_rect_.set(0, 0, 0, 0);
        }

        hor_rect_.left = map_hor_rect_.left - full_pan_offset_x_;
        hor_rect_.right = map_hor_rect_.right - full_pan_offset_x_;
        hor_rect_.top = map_hor_rect_.top - full_pan_offset_y_;
        hor_rect_.bottom = map_hor_rect_.bottom - full_pan_offset_y_;

        if (ret) {
            vert_rect_.left = map_vert_rect_.left - full_pan_offset_x_;
            vert_rect_.right = map_vert_rect_.right - full_pan_offset_x_;
            vert_rect_.top = map_vert_rect_.top - full_pan_offset_y_;
            vert_rect_.bottom = map_vert_rect_.bottom - full_pan_offset_y_;
        }

    }
    @Override
    public void draw() {
        drawing_ = true;
        int devrect[] = new int[8];
        int maprect[] = new int[8];

        devrect[0] = hor_rect_.left;
        devrect[1] = hor_rect_.top;
        devrect[2] = hor_rect_.right;
        devrect[3] = hor_rect_.bottom;

        devrect[4] = vert_rect_.left;
        devrect[5] = vert_rect_.top;
        devrect[6] = vert_rect_.right;
        devrect[7] = vert_rect_.bottom;

        maprect[0] = map_hor_rect_.left;
        maprect[1] = map_hor_rect_.top;
        maprect[2] = map_hor_rect_.right;
        maprect[3] = map_hor_rect_.bottom;

        maprect[4] = map_vert_rect_.left;
        maprect[5] = map_vert_rect_.top;
        maprect[6] = map_vert_rect_.right;
        maprect[7] = map_vert_rect_.bottom;
        //copyMapGraphicsN(this.getHandle(), full_pan_offset_x_, full_pan_offset_y_);
        eraseForDrawN(this.getHandle(), full_pan_offset_x_, full_pan_offset_y_, devrect);
        drawer_.onBeginTaskDraw(this);
        drawPanRectN(this.getHandle(), full_pan_offset_x_, full_pan_offset_y_, devrect, maprect);
        // ///////////////////////////////
        drawing_ = false;

		/*if(!continue_){
			if(!isCancelDrawN(this.getHandle())){
				drawer_.onFinishedTask(this, false);
				new_task_ = false;
				}
		}
		else
		{
			drawer_.onFinishedTask(this, false);
			new_task_ = false;
		}*/
        if(continue_){
            drawer_.onFinishedTask(this, false);
            new_task_ = false;
        }
    }

    public void setDraw() {
        continue_ = true;
        setDrawN(this.getHandle());


    }
    @Override
    public void cancelDraw(boolean bWait) {
        continue_ = false;
        cancelDrawN(this.getHandle(), bWait);


    }

    @Override
    public boolean isDrawind() {
        // TODO Auto-generated method stub
        return drawing_;
    }

    @Override
    public TypeTask getType() {
        // TODO Auto-generated method stub
        return type_;
    }

    @Override
    protected void DeleteNativeHandle(int handle) {
        // TODO Auto-generated method stub

    }

    public int GetPanOffsetX() {
        return pan_offset_x_;
    }

    public int GetPanOffsetY() {
        return pan_offset_y_;
    }

    public int GetPanFullOffsetX() {
        return full_pan_offset_x_;
    }

    public int GetPanFullOffsetY() {
        return full_pan_offset_y_;
    }

    boolean difference_rect(Rect rcWnd, Rect rcPan, Rect rcHor, Rect rcVer) {
        if (!Rect.intersects(rcWnd, rcPan))
            return false;

        if (rcWnd.left < rcPan.left) {
            rcHor.left = rcPan.left;
            rcHor.right = rcPan.right;

            rcVer.left = rcWnd.right;
            rcVer.right = rcPan.right;
        } else {
            rcHor.left = rcPan.left;
            rcHor.right = rcPan.right;

            rcVer.left = rcPan.left;
            rcVer.right = rcWnd.left;
        }

        if (rcWnd.top < rcPan.top) {
            rcHor.top = rcWnd.bottom;
            rcHor.bottom = rcPan.bottom;

            rcVer.top = rcPan.top;
            rcVer.bottom = rcPan.bottom;
        } else {
            rcHor.top = rcPan.top;
            rcHor.bottom = rcWnd.top;

            rcVer.top = rcPan.top;
            rcVer.bottom = rcPan.bottom;
        }

        if (rcHor.top == rcVer.top) {
            rcVer.top = rcHor.bottom;
        } else {
            rcVer.bottom = rcHor.top;
        }
        return true;
    }

    // //////////////////////native/////////////////////////////////////////////////////////////////////////////////


    private native void setDrawN(int handle);

    private native void cancelDrawN(int handle, boolean bWait);
    private native boolean isCancelDrawN(int handle);

    private native void eraseForDrawN(int handle, int panx, int pany,
                                      int devrect[]);

    private native void drawPanRectN(int handle, int panx, int pany,
                                     int devrect[], int maprect[]);
}

