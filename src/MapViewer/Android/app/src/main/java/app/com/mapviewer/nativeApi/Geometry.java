package app.com.mapviewer.nativeApi;

import android.graphics.Point;

public class Geometry {
    public static final int	POINT_TYPE = 0;
    public static final int	MULTI_POINT_TYPE = 1;
    public static final int	LINE_TYPE = 2;
    public static final int	POLYLINE_TYPE = 3;
    public static final int	POLYGON_TYPE = 4;

    private int geometry_type_;
    private Point [] points_;
    private int size_;

    public Geometry(int type, int size,  Point [] points){
        geometry_type_ = type;
        points_ = points;
        size_ = size;
    }

    public int getType(){
        return geometry_type_;
    }
    public Point [] getPoints(){
        return 	points_;
    }
    public int getSize(){
        return size_;
    }
}

