#ifndef _DATASET_LITE_I_SHAPE_CURSOR_H_
#define _DATASET_LITE_I_SHAPE_CURSOR_H_

#include "SpatialDataset.h"

namespace DatasetLite
{
	template<class _TIterator, class TZOrderVal>
	class TShapeCursorBase : public IShapeCursor
	{
	public:
		typedef typename TZOrderVal::TPointType TPointType;
		typedef _TIterator TIterator;

		TShapeCursorBase(TIterator& iterator, double dOffsetX, double dOffsetY, double dScaleX, double dScaleY) :
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
		
	protected:
		TIterator m_Iterator;
		double m_dOffsetX;
		double m_dOffsetY;
		double m_dScaleX;
		double m_dScaleY;
		
	};


	template<class TIterator, class TZOrderVal>
	class TShapeCursorRect : public TShapeCursorBase<TIterator, TZOrderVal>
	{
	public:
		typedef TShapeCursorBase<TIterator, TZOrderVal> TBase;
		typedef typename TBase::TPointType TPointType;
		
		TShapeCursorRect(TIterator& iterator, double dOffsetX, double dOffsetY, double dScaleX, double dScaleY) :
			TBase(iterator, dOffsetX, dOffsetY, dScaleX, dScaleY)
			{}

		
		virtual  CommonLib::bbox extent() const
		{
			const TZOrderVal& zVal = this->m_Iterator.key();
			TPointType xMin = 0, xMax = 0, yMin = 0, yMax = 0;
			zVal.getXY(xMin, yMin, xMax, yMax);
			CommonLib::bbox bbox;
			bbox.xMin = (xMin*this->m_dScaleX) - this->m_dOffsetX;
			bbox.xMax = (xMax*this->m_dScaleX) - this->m_dOffsetX;
			bbox.yMin = (yMin*this->m_dScaleY) - this->m_dOffsetY;
			bbox.yMax = (yMax*this->m_dScaleY) - this->m_dOffsetY;

			return bbox;
		}
	
	};


	template<class TIterator, class TZOrderVal>
	class TShapeCursorPoint : public TShapeCursorBase<TIterator, TZOrderVal>
	{
	public:
		typedef TShapeCursorBase<TIterator, TZOrderVal> TBase;
		typedef typename TBase::TPointType TPointType;

		TShapeCursorPoint(TIterator& iterator, double dOffsetX, double dOffsetY, double dScaleX, double dScaleY) :
		TBase(iterator, dOffsetX, dOffsetY, dScaleX, dScaleY)
		{}


		virtual  CommonLib::bbox extent() const
		{
			CommonLib::bbox bbox;
			if(this->m_Iterator.isNull())
			{
				return bbox;
			}
			const TZOrderVal& zVal = this->m_Iterator.key();
			TPointType X = 0, Y = 0;
			zVal.getXY(X, Y);
			
			bbox.xMax = bbox.xMin = (X*this->m_dScaleX) - this->m_dOffsetX;
			bbox.yMax = bbox.yMin = (Y*this->m_dScaleY) - this->m_dOffsetY;
			return bbox;
		}

	};


}

#endif