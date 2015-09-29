#ifndef _DATASET_LITE_I_SHAPE_CURSOR_H_
#define _DATASET_LITE_I_SHAPE_CURSOR_H_

#include "SpatialDataset.h"

namespace DatasetLite
{
	template<class TIterator, class TZOrderVal>
	class TShapeCursor : public IShapeCursor
	{
	public:
		typedef typename TZOrderVal::TPointType TPointType;

		TShapeCursor(TIterator& iterator, double dOffsetX, double dOffsetY, double dScaleX, double dScaleY) :
		  m_Iterator(iterator),  m_dOffsetX(dOffsetX),  m_dOffsetY(dOffsetY),  m_dScaleX(dScaleX),  m_dScaleY(dScaleY)
		{

		}

		virtual bool next()
		{
			return m_Iterator.next();
		}
		virtual int row() const
		{
			return m_Iterator.value();
		}
		virtual bool IsEnd() const
		{
			return m_Iterator.isNull();
		}
		virtual  CommonLib::bbox extent() const
		{
			const TZOrderVal& zVal = m_Iterator.key();
			TPointType xMin = 0, xMax = 0, yMin = 0, yMax = 0;
			zVal.getXY(xMin, yMin, xMax, yMax);
			CommonLib::bbox bbox;
			bbox.xMin = (xMin*m_dScaleX) - m_dOffsetX;
			bbox.xMax = (xMax*m_dScaleX) - m_dOffsetX;
			bbox.yMin = (yMin*m_dScaleY) - m_dOffsetY;
			bbox.yMax = (yMax*m_dScaleY) - m_dOffsetY;

			return bbox;
		}
	private:
		TIterator m_Iterator;
		double m_dOffsetX;
		double m_dOffsetY;
		double m_dScaleX;
		double m_dScaleY;
		
	};

}

#endif