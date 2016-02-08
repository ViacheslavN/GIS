package app.com.mapviewer.nativeApi;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


public class DrawindThread extends Thread {

    private IDrawTask task_ = null;
    private Object event_ = null;
    //private Condition event_ = null;
    //private Lock lock_ = null;

    public DrawindThread(){
        event_ = new Object();
        //lock_ = new ReentrantLock();
        // event_ = lock_.newCondition();
    }
    public void run() {
        try {
            while(true)
            {
                synchronized(event_){
                    event_.wait();
                }

                if(Thread.interrupted())
                    break;

                synchronized(this){
                    if(task_ != null)
                    {
                        task_.draw();
                    }

                }

                if(Thread.interrupted())
                    break;
            }
        } catch (InterruptedException ex) {

        }
    }

    public void  setTask(IDrawTask task, boolean bDraw){
        synchronized(this){
            task_ = task;
        }
        if(bDraw){
            synchronized(event_){
                event_.notify();
            }
        }

    }

    public void  stopDraw( boolean bWait,  boolean bClearTask){

        if(task_ == null)
            return;
        task_.cancelDraw(bWait);

        if(bWait){
            synchronized(this){
                if(bClearTask){
                    task_ = null;
                }
            }
        }
        else
        {
            if(bClearTask){
                task_ = null;
            }
        }
    }
}
