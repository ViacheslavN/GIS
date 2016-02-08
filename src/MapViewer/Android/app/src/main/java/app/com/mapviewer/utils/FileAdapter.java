package app.com.mapviewer.utils;


import app.com.mapviewer.R;
import java.io.File;
import java.util.ArrayList;





import android.app.Activity;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

public class FileAdapter extends BaseAdapter {

    private ArrayList<File> Files_;
    private static LayoutInflater inflater_ = null;
    private Activity activity_;
    private Bitmap dirBmp_;
    private Bitmap dirupBmp_;
    private Bitmap mapfileBmp_;

    public FileAdapter(Activity a, ArrayList<File> Files)
    {
        activity_ = a;
        Files_ = Files;
        inflater_ = (LayoutInflater)activity_.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

        dirBmp_ = BitmapFactory.decodeResource(activity_.getResources(), R.drawable.directory);
        dirupBmp_ = BitmapFactory.decodeResource(activity_.getResources(), R.drawable.updirectory);
        mapfileBmp_ = BitmapFactory.decodeResource(activity_.getResources(), R.drawable.mapfile);
    }


    public static class ViewHolder{
        public TextView text;
        public ImageView image;
    }

    public int getCount() {
        return Files_.size();
    }

    public Object getItem(int position) {
        return position;
    }

    public long getItemId(int position) {
        return position;
    }

    public View getView(int position, View convertView, ViewGroup parent) {
        View vi = convertView;
        ViewHolder holder;
        if(convertView == null)
        {
            vi = inflater_.inflate(R.layout.file_list_view, null);
            holder = new ViewHolder();
            holder.text = (TextView)vi.findViewById(R.id.text);;
            holder.image = (ImageView)vi.findViewById(R.id.image);
            vi.setTag(holder);
        }
        else
            holder=(ViewHolder)vi.getTag();

        String fileName = Files_.get(position).getName();
        holder.text.setText(fileName);
        if(fileName.equals(".."))
        {
            holder.image.setImageBitmap(dirupBmp_);
        }
        else if(Files_.get(position).isDirectory())
        {
            holder.image.setImageBitmap(dirBmp_);
        }
        else
        {
            holder.image.setImageBitmap(mapfileBmp_);
        }
        return vi;
    }
}

