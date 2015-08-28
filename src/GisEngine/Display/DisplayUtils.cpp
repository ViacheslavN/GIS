#include "stdafx.h"
#include "DisplayUtils.h"
#include "../agg/agg_trans_affine.h"
namespace GisEngine
{
	namespace Display
	{


		const static double dInchesPerMillimeters = 0.03937;

		double Round(double num)
		{
			return floor(num + 0.5);
		}

		GUnits SymbolSizeToDeviceSize(IDisplayTransformation *trans, double sizeSymb, bool scaleDependent)
		{

			if (!trans || !sizeSymb)
				return static_cast<GUnits>(Round(sizeSymb));

			double scale = (scaleDependent && trans->GetScale() && trans->UseReferenceScale())
				? (static_cast<double>(trans->GetReferenceScale()) / static_cast<double>(trans->GetScale()))
				: 1.0;
			return SymbolSizeToDeviceSize(trans->GetResolution() * scale, sizeSymb);
		}

		double DeviceSizeToSymbolSize(IDisplayTransformation *trans, GUnits sizeDev, bool scaleDependent)
		{

			if (!trans || !sizeDev)
				return static_cast<double>(sizeDev);

			double scale = (scaleDependent && trans->GetScale() && trans->UseReferenceScale())
				? (static_cast<double>(trans->GetReferenceScale()) / static_cast<double>(trans->GetScale()))
				: 1.0;
			return DeviceSizeToSymbolSize(trans->GetResolution() * scale, sizeDev);
		}

		GUnits SymbolSizeToDeviceSize(double dpi, double sizeSymb)
		{
			return (GUnits)(dpi * dInchesPerMillimeters * sizeSymb);
		}

		double DeviceSizeToSymbolSize(double dpi, GUnits sizeDev)
		{
			return (GUnits)(sizeDev / (dpi * dInchesPerMillimeters));
		}

		void RotateCoords(const GPoint& center, double angle, GPoint* points, int count)
		{
			agg::trans_affine tr;
			tr *= agg::trans_affine_translation(-center.x, -center.y);
			tr *= agg::trans_affine_rotation(DEG2RAD(angle));
			tr *= agg::trans_affine_translation(center.x, center.y);

			for(int i = 0; i < count; i++)
			{
				double x = points[i].x;
				double y = points[i].y;
				tr.transform(&x, &y);
				points[i].x = (GUnits)x;
				points[i].y = (GUnits)y;
			}
		}

	}
}
