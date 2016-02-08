package app.com.mapviewer.utils;

import android.view.View;
import android.widget.AdapterView;
public  abstract class  OnFileDlgListener
{
    static public class FileDlgResult
    {
        public final String fileName;
        public final String filePath;
        public final String AbsolutePath;

        public 	FileDlgResult(String fN, String fP, String aP){
            fileName = fN;
            filePath = fP;
            AbsolutePath = aP;
        }

    }
    public abstract void OnFileCloseDlg(FileDlgResult fdr);
}