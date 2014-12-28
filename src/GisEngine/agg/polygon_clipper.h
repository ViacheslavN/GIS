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
  GisEngine::GisBoundingBox m_clip_box_;
  std::vector<ClipEdge> clipEdges_;

  struct IntervalInfo
  {
    double xMin;
    double xMax;
    bool inside;
    std::vector<int> intervalEdges;
  };
  struct StripInfo
  {
    std::vector<IntervalInfo> intervalEdges;
  };
  std::vector<StripInfo> strips_;
  int stripCount_;
  bool inited_;

  void  calc_strip_count();
  void  make_strips();
  void  calc_interval_borders(double stripMinY, double stripMaxY, std::vector<double> &intervalBorders);
  void  solve_strips();
  void  calc_edge_prj(ClipEdge &clipEdge, double stripMinY, double stripMaxY, double &xInt1, double &xInt2);
};
}
#endif