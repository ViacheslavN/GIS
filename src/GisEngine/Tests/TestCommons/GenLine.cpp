#include "stdafx.h"
#include "../../agg/agg_vcgen_stroke.h"

#include "LineGenerator.h"


double cross_product(const GisEngine::Display::GPoint& pt1,
	const GisEngine::Display::GPoint& pt2,
	const GisEngine::Display::GPoint& pt3)
{
	return (pt3.x - pt2.x) * (pt2.y - pt1.y) - (pt3.y - pt2.y) * (pt2.x - pt1.x);
}

double cross_product(double x1, double y1,
	double x2, double y2,
	double x, double y)
{
	return (x - x2) * (y2 - y1) - (y - y2) * (x2 - x1);
}

void GenLine()
{

	GisEngine::Display::GPoint pt1(10, 10);
	GisEngine::Display::GPoint pt2(15, 40);
	GisEngine::Display::GPoint pt3(50, 20);

	double d = cross_product(pt1, pt2, pt3);


	GisEngine::Display::GPoint pt4(10, 10);
	GisEngine::Display::GPoint pt5(15, 3);
	GisEngine::Display::GPoint pt6(50, 20);

	double d1 = cross_product(pt4, pt5, pt6);

	GisEngine::Display::CLineGenerator lineGen;

	std::vector<GisEngine::Display::GPoint> vecPoint;

	vecPoint.push_back(GisEngine::Display::GPoint(10, 10));
	vecPoint.push_back(GisEngine::Display::GPoint(10, 20));
	vecPoint.push_back(GisEngine::Display::GPoint(10, 30));
//	vecPoint.push_back(GisEngine::Display::GPoint(50, 50));
	lineGen.SetWidth(5);

	lineGen.CreateLine(vecPoint);

	agg::vcgen_stroke stroke;

	stroke.add_vertex(10, 10, agg::path_cmd_move_to);
	stroke.add_vertex(10, 20, agg::path_cmd_line_to);
	stroke.add_vertex(10, 30, agg::path_cmd_line_to);
//	stroke.add_vertex(50, 50, agg::path_cmd_line_to);
	//stroke.add_vertex(10, 10, agg::path_flags_close);
	stroke.width(5);
	//stroke.add_vertex(50, 180, agg::path_cmd_line_to);
	//stroke.add_vertex(50, 180, agg::path_cmd_line_to);


	double dX, dY;

	std::vector<std::pair<double, double>> vecPoints;

	while (true)
	{
		int nCMP = stroke.vertex(&dX, &dY);
	
		if (nCMP == agg::path_cmd_stop || nCMP == (agg::path_cmd_end_poly | agg::path_flags_close | agg::path_flags_cw))
		{
			break;
		}
		vecPoints.push_back(std::make_pair(dX, dY));
	}

	
}