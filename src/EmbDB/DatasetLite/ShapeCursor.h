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

		TShapeCursorBase(TIterator& iterator, double dOffsetX, double dOffsetY, byte nScaleX, byte nScaleY) :
		  m_Iterator(iterator),  m_dOffsetX(dOffsetX),  m_dOffsetY(dOffsetY),  m_nScaleX(nScaleX),  m_nScaleY(nScaleY)
		{
			m_dCalcScaleX = 1/pow(10., m_nScaleX);
			m_dCalcScaleY = 1/pow(10., m_nScaleY);
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
		byte m_nScaleX;
		byte m_nScaleY;

		double m_dCalcScaleX;
		double m_dCalcScaleY;
		
	};


	template<class TIterator, class TZOrderVal>
	class TShapeCursorRect : public TShapeCursorBase<TIterator, TZOrderVal>
	{
	public:
		typedef TShapeCursorBase<TIterator, TZOrderVal> TBase;
		typedef typename TBase::TPointType TPointType;
		
		TShapeCursorRect(TIterator& iterator, double dOffsetX, double dOffsetY, byte nScaleX, byte nScaleY) :
			TBase(iterator, dOffsetX, dOffsetY, nScaleX, nScaleY)
			{}

		
		virtual  CommonLib::bbox extent() const
		{
			const TZOrderVal& zVal = this->m_Iterator.key();
			TPointType xMin = 0, xMax = 0, yMin = 0, yMax = 0;
			zVal.getXY(xMin, yMin, xMax, yMax);
			CommonLib::bbox bbox;
			bbox.xMin = (xMin*this->m_dCalcScaleX) - this->m_dOffsetX;
			bbox.xMax = (xMax*this->m_dCalcScaleX) - this->m_dOffsetX;
			bbox.yMin = (yMin*this->m_dCalcScaleY) - this->m_dOffsetY;
			bbox.yMax = (yMax*this->m_dCalcScaleY) - this->m_dOffsetY;

			return bbox;
		}
	
	};


	template<class TIterator, class TZOrderVal>
	class TShapeCursorPoint : public TShapeCursorBase<TIterator, TZOrderVal>
	{
	public:
		typedef TShapeCursorBase<TIterator, TZOrderVal> TBase;
		typedef typename TBase::TPointType TPointType;

		TShapeCursorPoint(TIterator& iterator, double dOffsetX, double dOffsetY, byte nScaleX, byte nScaleY) :
		TBase(iterator, dOffsetX, dOffsetY, nScaleX, nScaleY)
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
			
			bbox.xMax = bbox.xMin = (X*this->m_dCalcScaleX) - this->m_dOffsetX;
			bbox.yMax = bbox.yMin = (Y*this->m_dCalcScaleY) - this->m_dOffsetY;
			return bbox;
		}

	};


}

#endif