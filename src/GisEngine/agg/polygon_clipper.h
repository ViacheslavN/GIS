#ifndef AGG_POLYGON_CLIPPER
#define AGG_POLYGON_CLIPPER

#include "agg_basics.h"
#include "agg_rendering_buffer.h"

namespace agg
{

class polygon_clipper
{
public:
  polygon_clipper();
  ~polygon_clipper();

  void  set_edges(const point_d* lpPoints, const int *lpPolyCounts, int nCount);
  bool  isInside(double x, double y);
  void  reset();

  struct ClipEdge
  {
    // y1 <= y2
    double x1;
    double y1;
    double x2;
    double y2;
    double k;
    double yk;
  };

private:
  UGLite::GisBoundingBox m_clip_box_;
  decore::de_vector<ClipEdge> clipEdges_;

  struct IntervalInfo
  {
    double xMin;
    double xMax;
    bool inside;
    decore::de_vector<int> intervalEdges;
  };
  struct StripInfo
  {
    decore::de_vector<IntervalInfo> intervalEdges;
  };
  decore::de_vector<StripInfo> strips_;
  int stripCount_;
  bool inited_;

  void  calc_strip_count();
  void  make_strips();
  void  calc_interval_borders(double stripMinY, double stripMaxY, decore::de_vector<double> &intervalBorders);
  void  solve_strips();
  void  calc_edge_prj(ClipEdge &clipEdge, double stripMinY, double stripMaxY, double &xInt1, double &xInt2);
};
}
#endif