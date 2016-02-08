package app.com.mapviewer.nativeApi;

import java.util.Map;
public class Layers extends CObject {

    private java.util.TreeMap<Integer, Layer> layers_;
    private final int FEATURE_LAYER = 0;
    private final int GROUP_LAYER = 1;

    public Layers(int handle) {
        super(handle);
        // TODO Auto-generated constructor stub

        layers_ = new java.util.TreeMap<Integer, Layer>();
    }

    @Override
    protected void DeleteNativeHandle(int handle) {
        // TODO Auto-generated method stub

    }

    public  int    GetLayerCount(){
        return GetLayerCountN(this.getHandle());
    }
    public Layer GetLayer(int index){
        if(layers_.containsKey(index)){
            return layers_.get(index);
        }
        int arr[] = new int[2];

        int ptr = GetLayerN(this.getHandle(), index, arr);
        Layer layer = null;
        if(ptr != 0){

            switch( arr[0])
            {
                case FEATURE_LAYER:
                    layer = new FeatureLayer(ptr);
                    break;
                case GROUP_LAYER:
                    layer = new GroupLayer(ptr);
                    break;
                default:
                    layer = new Layer(ptr);
                    break;
            }


            layers_.put(index, layer);
            return layer;
        }
        return null;
    }
    public void   AddLayer(Layer layer){

    }
    public void   InsertLayer(Layer layer, int new_pos){

    }
    public void   RemoveLayer(Layer layer){

    }
    public  void   RemoveAllLayers(){

    }
    ///////////////////////////////////////////native//////////////////////////////////////////////////////
    private native int GetLayerCountN(int handle);
    private native int GetLayerN(int handle, int index, int arr[]);

}