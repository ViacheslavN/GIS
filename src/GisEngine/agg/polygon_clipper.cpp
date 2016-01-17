#include "polygon_clipper.h"
#include "stdafx.h"
#ifndef ANDROID
	#include <xutility>
#endif
namespace agg
{
polygon_clipper::polygon_clipper() : inited_(false)
{
}

polygon_clipper::~polygon_clipper()
{
}

void polygon_clipper::set_edges(const point_d* lpPoints, const int *lpPolyCounts, int nCount)
{
  inited_ = true;
  clipEdges_.clear();

  // считаем ожидаемое количество ребер
  int pointCount = 0;
  for(int ind = 0; ind < nCount; ++ind)
    pointCount += lpPolyCounts[ind];
  clipEdges_.reserve(2 * pointCount + nCount);

  m_clip_box_.xMin = lpPoints[0].x;
  m_clip_box_.yMin = lpPoints[0].y;
  m_clip_box_.xMax = lpPoints[0].x;
  m_clip_box_.yMax = lpPoints[0].y;
  
  ClipEdge clipEdge;
  int indCount = 0;
  for(int ind = 0; ind < nCount; ++ind)
  {
    for(int pointInd = indCount; pointInd < indCount + lpPolyCounts[ind]; ++pointInd)
    {
      // преобразуем точки в ребра
      clipEdge.x1 = lpPoints[pointInd].x;
      clipEdge.y1 = lpPoints[pointInd].y;
      if(pointInd < indCount + lpPolyCounts[ind] - 1)
      {
        clipEdge.x2 = lpPoints[pointInd + 1].x;
        clipEdge.y2 = lpPoints[pointInd + 1].y;
      }
      else
      {
        clipEdge.x2 = lpPoints[indCount].x;
        clipEdge.y2 = lpPoints[indCount].y;
      }

      if(clipEdge.y1 > clipEdge.y2)
      {
        std::swap(clipEdge.x1, clipEdge.x2);
        std::swap(clipEdge.y1, clipEdge.y2);
      }

      // коэффициенты для расчета пересечений отрезков
      if((clipEdge.x1 == clipEdge.x2) && (clipEdge.y1 == clipEdge.y2))
        continue;

      // определяем общий экстент
      if(m_clip_box_.xMin > lpPoints[pointInd].x) m_clip_box_.xMin = lpPoints[pointInd].x;
      if(m_clip_box_.yMin > lpPoints[pointInd].y) m_clip_box_.yMin = lpPoints[pointInd].y;
      if(m_clip_box_.xMax < lpPoints[pointInd].x) m_clip_box_.xMax = lpPoints[pointInd].x;
      if(m_clip_box_.yMax < lpPoints[pointInd].y) m_clip_box_.yMax = lpPoints[pointInd].y;

      if(clipEdge.y2 != clipEdge.y1)
      {
        clipEdge.k = (clipEdge.x2 - clipEdge.x1) / (clipEdge.y2 - clipEdge.y1);
        clipEdge.yk = clipEdge.y1 * clipEdge.k;
      }
      else
      {
        clipEdge.k = 0;
        clipEdge.yk = 0;
      }
      clipEdges_.push_back(clipEdge);
    }
    indCount += lpPolyCounts[ind];
  }

  m_clip_box_.xMin -= 0.2;
  m_clip_box_.yMin -= 0.2;
  m_clip_box_.xMax += 0.2;
  m_clip_box_.yMax += 0.2;

  calc_strip_count();
  make_strips();
  solve_strips();
}

void polygon_clipper::calc_strip_count()
{
  // эвристика для определения количества полос
  // отбрасываем 10% сверху и снизу и считаем среднее ребро
  // принимаем размер полосы за размер среднего ребра
  int edgeCount  = clipEdges_.size();
  std::vector<double> lenCalc;
  lenCalc.reserve(edgeCount);
  for(int ind = 0; ind < edgeCount; ++ind)
  {
    ClipEdge &clipEdge = clipEdges_[ind];
    double edgeLen = sqrt((clipEdge.x2 - clipEdge.x1) * (clipEdge.x2 - clipEdge.x1) + (clipEdge.y2 - clipEdge.y1) * (clipEdge.y2 - clipEdge.y1));
    lenCalc.push_back(edgeLen);
  }
  int offset = edgeCount / 10;
  if(offset < 1)
    offset = 1;
  if(edgeCount - 2 * offset <= 0)
    offset = 0;

  double totalLen = 0;
  int counter = 0;
  for(int ind = offset; ind < edgeCount - offset; ++ind)
  {
    ++counter;
    totalLen += lenCalc[ind];
  }
  double stripLen = totalLen / double(counter);
  stripCount_ = (int)((m_clip_box_.yMax - m_clip_box_.yMin + 1) / stripLen);

  // на всякий случай
  if(stripCount_ < 5)
    stripCount_ = 5;
}

void polygon_clipper::calc_edge_prj(ClipEdge &clipEdge, double stripMinY, double stripMaxY, double &xInt1, double &xInt2)
{
  // x-координаты проекции ребра на границы полосы
  double minX = min(clipEdge.x1, clipEdge.x2);
  double maxX = max(clipEdge.x1, clipEdge.x2);
  xInt1 = clipEdge.x1 + stripMinY * clipEdge.k - clipEdge.yk;
  if(clipEdge.k == 0)
    xInt1 = minX;
  if(xInt1 < minX) xInt1 = minX;
  if(xInt1 > maxX) xInt1 = maxX;
  xInt2 = clipEdge.x1 + stripMaxY * clipEdge.k - clipEdge.yk;
  if(clipEdge.k == 0)
    xInt2 = maxX;
  if(xInt2 < minX) xInt2 = minX;
  if(xInt2 > maxX) xInt2 = maxX;
  if(xInt1 > xInt2) std::swap(xInt1, xInt2);
  xInt1 -= 0.1;
  xInt1 += 0.1;
}

void polygon_clipper::calc_interval_borders(double stripMinY, double stripMaxY, std::vector<double> &intervalBorders)
{
  int edgeCount  = clipEdges_.size();
  for(int ind = 0; ind < edgeCount; ++ind)
  {
    ClipEdge &clipEdge = clipEdges_[ind];
    if(clipEdge.y1 <= stripMaxY && clipEdge.y2 >= stripMinY)
    {
      // x-координаты проекции ребра на границы полосы
      double xInt1;
      double xInt2;
      calc_edge_prj(clipEdge, stripMinY, stripMaxY, xInt1, xInt2);
      
      bool found = true;
      bool needInsert = false;
      bool foundAny = false;
      while(found)
      {
        found = false;
        // объединяем все интервалы которые пересекаются с новым
        for(size_t intInd = 0; intInd < intervalBorders.size(); intInd += 2)
        {
          if((intervalBorders[intInd] <= xInt1 && xInt1 <= intervalBorders[intInd + 1]) ||
            (intervalBorders[intInd] <= xInt2 && xInt2 <= intervalBorders[intInd + 1]) || 
            (xInt1 <= intervalBorders[intInd] && intervalBorders[intInd] <= xInt2) ||
            (xInt1 <= intervalBorders[intInd + 1] && intervalBorders[intInd + 1] <= xInt2))
          {
            foundAny = true;
            xInt1 = min(intervalBorders[intInd], xInt1);
            xInt2 = max(intervalBorders[intInd + 1], xInt2);
            if(xInt1 < intervalBorders[intInd] || xInt2 > intervalBorders[intInd + 1])
            {
              if(intervalBorders.size() - intInd > 2)
                memcpy(&intervalBorders[intInd], &intervalBorders[intInd + 2], (intervalBorders.size() - intInd - 2) * sizeof(double));
              intervalBorders.pop_back();
              intervalBorders.pop_back();
              needInsert = true;
            }
            else
              break;
            found = true;
            break;
          }
        }
      }

      if(needInsert || !foundAny)
      {
        intervalBorders.push_back(xInt1);
        intervalBorders.push_back(xInt2);
        for(size_t intInd = 0; intInd < intervalBorders.size(); intInd += 2)
          if(xInt2 <= intervalBorders[intInd])
        {
          for(int copyInd = (int)intervalBorders.size() - 1; copyInd >= (int)intInd + 3; -- copyInd)
          {
            intervalBorders[copyInd] = intervalBorders[copyInd - 2];
            intervalBorders[copyInd - 1] = intervalBorders[copyInd - 3];
          }
          intervalBorders[intInd] = xInt1;
          intervalBorders[intInd + 1] = xInt2;
          break;
        }
      }
    }
  }
  if(intervalBorders.empty())
  {
    intervalBorders.push_back(m_clip_box_.xMin);
    intervalBorders.push_back(m_clip_box_.xMax);
  }
}

void polygon_clipper::make_strips()
{
  strips_.clear();
  strips_.reserve(stripCount_);
  double stripLen = (m_clip_box_.yMax - m_clip_box_.yMin + 1) / double(stripCount_);
  int edgeCount  = clipEdges_.size();

  for(int ind = 0; ind < stripCount_; ++ind)
  {
    // разбиваем все ребра по полосам
    double stripMinY = stripLen * double(ind) + m_clip_box_.yMin;
    double stripMaxY = stripMinY + stripLen;
    strips_.push_back(StripInfo());
    std::vector<IntervalInfo> &currentStripIInfo = strips_[strips_.size() - 1].intervalEdges;
    currentStripIInfo.reserve(10);

    // до заполнения ребрами надо определить интервалы
    std::vector<double> intervalBorders;
    intervalBorders.reserve(2 * (edgeCount / stripCount_ + 10));
    calc_interval_borders(stripMinY, stripMaxY, intervalBorders);

    // наполняем интервалы ребрами
    // вставляем пустые "хорошие" интевалы
    currentStripIInfo.push_back(IntervalInfo());
    IntervalInfo &currInt = currentStripIInfo[currentStripIInfo.size() - 1];
    currInt.xMin = m_clip_box_.xMin;
    currInt.xMax = intervalBorders[0];
    for(int intInd = 0; intInd < (int)intervalBorders.size(); intInd += 2)
    {
      currentStripIInfo.push_back(IntervalInfo());
      IntervalInfo &currInt = currentStripIInfo[currentStripIInfo.size() - 1];
      currInt.xMin = intervalBorders[intInd];
      currInt.xMax = intervalBorders[intInd + 1];

      std::vector<int> &currentStripInterval = currInt.intervalEdges;
      currentStripInterval.reserve(edgeCount / stripCount_ + 10);
      for(int ind = 0; ind < edgeCount; ++ind)
      {
        ClipEdge &clipEdge = clipEdges_[ind];
        if(stripMinY <= clipEdge.y1 && stripMaxY >= clipEdge.y1 || 
            stripMinY <= clipEdge.y2 && stripMaxY >= clipEdge.y2 ||
            clipEdge.y1 <= stripMinY && stripMinY <= clipEdge.y2 ||
            clipEdge.y1 <= stripMaxY && stripMaxY <= clipEdge.y2)
        {
          double xMin;
          double xMax;
          calc_edge_prj(clipEdge, stripMinY, stripMaxY, xMin, xMax);
          if(xMin <= intervalBorders[intInd] && xMax >= intervalBorders[intInd] || 
              xMin <= intervalBorders[intInd + 1] && xMax >= intervalBorders[intInd + 1] ||
              intervalBorders[intInd] <= xMin && xMin <= intervalBorders[intInd + 1] ||
              intervalBorders[intInd] <= xMax && xMax <= intervalBorders[intInd + 1])
          {
            currentStripInterval.push_back(ind);
          }
        }
      }

    currentStripIInfo.push_back(IntervalInfo());
    IntervalInfo &currInt2 = currentStripIInfo[currentStripIInfo.size() - 1];
    currInt2.xMin = intervalBorders[intInd + 1];
    if(intInd < (int)intervalBorders.size() - 2)
      currInt2.xMax = intervalBorders[intInd + 2];
    else
      currInt2.xMax = m_clip_box_.xMax;
    }
  }
}

inline double calcXCoord(polygon_clipper::ClipEdge &curEdge, double yCoord)
{
  return curEdge.x1 + yCoord * curEdge.k - curEdge.yk;
}

void polygon_clipper::solve_strips()
{
  double stripLen = (m_clip_box_.yMax - m_clip_box_.yMin + 1) / double(stripCount_);
  int edgeCount  = clipEdges_.size();
  for(int ind = 0; ind < stripCount_; ++ind)
  {
    // разбиваем все ребра по полосам
    double stripMinY = stripLen * double(ind) + m_clip_box_.yMin;
    double stripMaxY = stripMinY + stripLen;
    std::vector<IntervalInfo> &currentStripIInfo = strips_[ind].intervalEdges;
    for(size_t intInd = 0; intInd < currentStripIInfo.size(); intInd += 2)
    {
      IntervalInfo &currentStripInterval = currentStripIInfo[intInd];
      if(intInd == 0 || intInd == currentStripIInfo.size() - 1)
      {
        currentStripInterval.inside = false;
        if(intInd == currentStripIInfo.size() - 1 && currentStripIInfo.size() == 3)
        {
          // вырожденный случай
          IntervalInfo &leftStripInterval = currentStripIInfo[intInd - 1];
          int currentEdgeCount = leftStripInterval.intervalEdges.size();
          if(currentEdgeCount == 0)
            leftStripInterval.inside = false;
        }
      }
      else
      {
        double ptX = (currentStripInterval.xMin + currentStripInterval.xMax) / 2.;
        double ptY = (stripMinY + stripMaxY) / 2.;

        IntervalInfo &leftStripInterval = currentStripIInfo[intInd - 1];
        int currentEdgeCount = leftStripInterval.intervalEdges.size();
        int counter =0 ;
        for(int curEdgeInd = 0; curEdgeInd < currentEdgeCount; ++curEdgeInd)
        {
          ClipEdge &curEdge = clipEdges_[ leftStripInterval.intervalEdges[curEdgeInd] ];
          if(curEdge.y1 <= ptY && ptY < curEdge.y2)
          {
            double xInt = calcXCoord(curEdge, ptY);
            if(xInt <= ptX)
              ++counter;
          }
        }
        bool evenCounter = (((counter >> 1) << 1) == counter);
        if(evenCounter)
          currentStripInterval.inside = currentStripIInfo[intInd - 2].inside;
        else
          currentStripInterval.inside = !(currentStripIInfo[intInd - 2].inside);
      }
    }
  }
}

bool polygon_clipper::isInside(double x, double y)
{
  if(!inited_)
    return true;

  if(clipEdges_.empty())
    return false;
  if(x < m_clip_box_.xMin || x > m_clip_box_.xMax || y < m_clip_box_.yMin || y > m_clip_box_.yMax)
    return false;
  double stripLen = (m_clip_box_.yMax - m_clip_box_.yMin + 1) / double(stripCount_);
  int stripInd = (int)((y - m_clip_box_.yMin) / stripLen);
  if(stripInd < 0)
    stripInd =0;
  if(stripInd >= stripCount_)
    stripInd = stripCount_ - 1;

  //int counter =0 ;
  //for(int curEdgeInd = 0; curEdgeInd < clipEdges_.size(); ++curEdgeInd)
  //{
  //  ClipEdge &curEdge = clipEdges_[ curEdgeInd ];
  //  if(curEdge.y1 <= y && y < curEdge.y2)
  //  {
  //    double xInt = calcXCoord(curEdge, y);
  //    if(xInt <= x)
  //      ++counter;
  //  }
  //}
  //bool evenCounter = (((counter >> 1) << 1) == counter);
  //return !evenCounter;

  std::vector<IntervalInfo> &currentStripIInfo = strips_[stripInd].intervalEdges;
  for(size_t intInd = 0; intInd < currentStripIInfo.size(); intInd++)
  {
    IntervalInfo &currentStripInterval = currentStripIInfo[intInd];
    if(x < currentStripInterval.xMin || x >= currentStripInterval.xMax)
      continue;

    if(currentStripInterval.intervalEdges.empty())
      return currentStripInterval.inside;

    if(currentStripInterval.xMin <= x && x <= currentStripInterval.xMax)
    {
      int currentEdgeCount = currentStripInterval.intervalEdges.size();
      int counter =0 ;
      for(int curEdgeInd = 0; curEdgeInd < currentEdgeCount; ++curEdgeInd)
      {
        ClipEdge &curEdge = clipEdges_[ currentStripInterval.intervalEdges[curEdgeInd] ];
        if(curEdge.y1 <= y && y < curEdge.y2)
        {
          double xInt = calcXCoord(curEdge, y);
          if(xInt <= x)
            ++counter;
        }
      }
      bool evenCounter = (((counter >> 1) << 1) == counter);
      if(evenCounter)
        return currentStripIInfo[intInd - 1].inside;
      return !(currentStripIInfo[intInd - 1].inside);
    }
  }
  return false;
}

void polygon_clipper::reset()
{
  inited_ = false;
  clipEdges_.clear();
}
}