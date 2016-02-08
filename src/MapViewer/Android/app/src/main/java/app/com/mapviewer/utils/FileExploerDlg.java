package app.com.mapviewer.utils;
import java.io.File;
import java.util.ArrayList;

import app.com.mapviewer.R;
import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.Button;
import android.app.AlertDialog;
import android.widget.Spinner;
import android.widget.ArrayAdapter;
import android.widget.AdapterView.OnItemSelectedListener;
import android.view.View.OnClickListener;
import android.widget.AdapterView.OnItemClickListener;


public class FileExploerDlg
{
    private File CurrentDir_;
    private ArrayList<File> Files_;
    private String[] Filters_;
    //private String[] Dirs_;
    private ArrayList<String> Dirs_;
    private Dialog dialog_;
    private ListView filelistview_;
    private Spinner spinner_;
    private Activity activity_;
    private int max_spinner_level_ = 5;
    private boolean spin_change = false;
    private OnFileDlgListener OnfileDlgResult_ = null;

    public FileExploerDlg(Activity activity, OnFileDlgListener OnfileDlgResult) {

        Files_ = new ArrayList<File>();
        Filters_ = new String[3];
        //Dirs_ = new String[3];
        Dirs_ = new ArrayList<String>();

        Filters_[0] = "shp";
        Filters_[1] = "embDB";
        Filters_[2] = "sqlite";

        Dirs_.add("/mnt/sdcard");
        Dirs_.add("/mnt");
        Dirs_.add("/");
        activity_ = activity;
        OnfileDlgResult_ = OnfileDlgResult;
    }
    public void CreateDlg()
    {
        try
        {


            dialog_ = new Dialog(activity_/*,R.style.Dialog_Fullscreen*/);
            dialog_.setContentView(R.layout.file_exploer);
            dialog_.setTitle("File Exploer");


            filelistview_ = (ListView) dialog_.findViewById(R.id.filelist);
            filelistview_.setOnItemClickListener(sellistener);
            spinner_ = (Spinner) dialog_.findViewById(R.id.spinner);
            Button button = (Button)dialog_.findViewById(R.id.Cancel);
            button.setOnClickListener(new OnClickListener()
                                      {
                                          public void onClick(View v) {
                                              dialog_.hide();
                                          }
                                      }
            );

            spinner_.setOnItemSelectedListener(new OnItemSelectedListener() {
                public void onItemSelected(
                        AdapterView<?> parent, View view, int pos, long id)
                {
                    String dir = parent.getItemAtPosition(pos).toString();
                    browseTo(new File(dir), false);
                }
                public void onNothingSelected(AdapterView<?> parent) {
                    //showToast("Spinner1: unselected");
                }

            });

            ArrayAdapter<String> adapter = new ArrayAdapter<String>(activity_,
                    R.layout.spinne_dir, R.id.dirname, Dirs_);
            spinner_.setAdapter(adapter);
            // listview.setAdapter(selectedAdapter);
            dialog_.show();


        }
        catch(Exception ex)
        {
            String message = ex.getMessage();
            ex.printStackTrace();
        }
    }
    public OnItemClickListener sellistener = new OnItemClickListener(){
        public void onItemClick(
                AdapterView<?> parent, View view, int pos, long id)
        {
            // String dir = parent.getItemAtPosition(pos).toString();
            String Path =  Files_.get(pos).getPath();
            if(Path.equals(".."))
            {
                browseTo(new File(CurrentDir_.getParent()), true);
            }
            else
            {
                browseTo(Files_.get(pos), true);
            }
        }
        public void onNothingSelected(AdapterView<?> parent) {
            //showToast("Spinner1: unselected");
        }
    };
    private  void browseTo(File location, boolean add_dir)
    {
        Files_.clear();
        if(!location.exists())
        {
            FileAdapter   adapter = new FileAdapter(activity_, Files_);
            filelistview_.setAdapter(adapter);
            return;
        }
        if(location.isFile())
        {
            OnFileDlgListener.FileDlgResult fdr = new OnFileDlgListener.FileDlgResult(location.getName(), location.getPath(), location.getAbsolutePath());
            dialog_.hide();
            OnfileDlgResult_.OnFileCloseDlg(fdr);
            return;
        }

        CurrentDir_ = location;
        String path =  CurrentDir_.getPath();
        if(!add_dir)
        {
            Dirs_.remove(path);
            Dirs_.add(0, path);
        }
        else
        {
            if(Dirs_.contains(path))
            {
                Dirs_.remove(path);
                Dirs_.add(0, path);
            }
            else
            {
                Dirs_.add(0, path);
            }
            ArrayAdapter<String> adapter = new ArrayAdapter<String>(activity_,
                    R.layout.spinne_dir, R.id.dirname, Dirs_);
            spinner_.setAdapter(adapter);
        }
        if(location.getParentFile() != null)
            Files_.add(new File(".."));

        //dialog_.setTitle(CurrentDir_.getName().compareTo("") == 0 ? CurrentDir_.getPath() : CurrentDir_.getName());

        //if(location.getParentFile() != null) Files_.add(CurrentDir_.getParentFile());
        File[] filelist = CurrentDir_.listFiles();
        if(filelist != null)
        {
            for(File file : filelist)
            {
                if(file.isDirectory())
                {
                    Files_.add(file);
                }
                else if(Filters_ != null)
                {
                    for(String ext : Filters_)
                    {
                        if(file.getName().endsWith(ext))
                        {
                            Files_.add(file);
                            continue;
                        }
                    }
                }
                else
                {
                    Files_.add(file);
                }
            }
        }

        FileAdapter   adapter = new FileAdapter(activity_, Files_);
        filelistview_.setAdapter(adapter);
    }
}
