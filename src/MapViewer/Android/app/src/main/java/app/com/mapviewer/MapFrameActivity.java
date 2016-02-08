package app.com.mapviewer;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import app.com.mapviewer.utils.FileExploerDlg;
import app.com.mapviewer.utils.OnFileDlgListener;
import android.widget.LinearLayout;
public class MapFrameActivity extends AppCompatActivity {

    private MapView mapView = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

       /* LinearLayout pLayout = new LinearLayout(this);

        pLayout.add*/

        setContentView(R.layout.activity_map_frame);
        mapView = (MapView) findViewById(R.id.MapViewID);
        mapView.SetMapFrame(this);

        MapViewerApplication app =   (MapViewerApplication)this.getApplication();
        app.setMapFrame(this);
    }

    public void OnToolBtnClick(View theButton) {
        switch(theButton.getId())
        {
            case R.id.ShowSearchBtn:
                OnShowSearchView();
                break;
            case R.id.WhereIBtn:
                OnGPS();
                break;
            case R.id.FullScreenBtn:
                FullScreenBtn();
                break;
            case R.id.OpenMapBtn:
                OnOpenMap();
                break;
            case R.id.SettingsBtn:
                OnSettingsMenu();
                break;
        }
    }

    void  OnShowSearchView()
    {}
    void  OnGPS(){}
    void FullScreenBtn(){}
    void OnOpenMap()
    {
        FileExploerDlg dlg = new FileExploerDlg(this, filedlgListener);
        dlg.CreateDlg();

    }
    public OnFileDlgListener filedlgListener = new OnFileDlgListener()
    {
        public void OnFileCloseDlg(FileDlgResult fdr)
        {
            mapView.openMap(fdr.AbsolutePath);
        }
    };
    void OnSettingsMenu(){}
}
