#ifndef _EMBEDDED_DATABASE_RECT_ZORDER_COMPRESOR_H_
#define _EMBEDDED_DATABASE_RECT_ZORDER_COMPRESOR_H_


#include "../point/PointZOrderCompressor.h"


namespace embDB
{
	template<class _ZOrder, 
	class _TCoord,   uint32 _nMaxBitsLens, class _TCompressorParams = CompressorParamsBaseImp>
	class TRectZOrderCompressor : public TBaseZOrderCompressor<_ZOrder,_TCoord,  _nMaxBitsLens, _TCompressorParams>
	
	{
		public:
			typedef _ZOrder ZOrder;
			typedef _TCoord TCoord;
			typedef TBaseZOrderCompressor<_ZOrder,_TCoord, _nMaxBitsLens, _TCompressorParams> TBase;
			
			TRectZOrderCompressor(uint32 nPageSize, CommonLib::alloc_t* pAlloc = nullptr, _TCompressorParams *pCompParams = nullptr) :
				TBase(nPageSize, pAlloc, pCompParams)
			{
 
			}
			void AddSymbol(const ZOrder& zOrder)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				zOrder.getXY(xMin, yMin, xMax, yMax);
				this->m_PointEncoder.AddSymbol(xMin);
				this->m_PointEncoder.AddSymbol(yMin);
				this->m_PointEncoder.AddSymbol(xMax);
				this->m_PointEncoder.AddSymbol(yMax);
			}

			void RemoveSymbol(const ZOrder& zOrder)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				zOrder.getXY(xMin, yMin, xMax, yMax);
				this->m_PointEncoder.RemoveSymbol(xMin);
				this->m_PointEncoder.RemoveSymbol(yMin);
				this->m_PointEncoder.RemoveSymbol(xMax);
				this->m_PointEncoder.RemoveSymbol(yMax);
			}


			bool encodeSymbol(const ZOrder& zOrder)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				zOrder.getXY(xMin, yMin, xMax, yMax);
				if (!this->m_PointEncoder.encodeSymbol(xMin))
					return false;
				if (!this->m_PointEncoder.encodeSymbol(yMin))
					return false;
				if (!this->m_PointEncoder.encodeSymbol(xMax))
					return false;
				return this->m_PointEncoder.encodeSymbol(yMax);
			}


			void decodeSymbol(ZOrder& zOrder)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;

				this->m_PointEncoder.decodeSymbol(xMin);
				this->m_PointEncoder.decodeSymbol(yMin);
				this->m_PointEncoder.decodeSymbol(xMax);
				this->m_PointEncoder.decodeSymbol(yMax);
				zOrder.setZOrder(xMin, yMin, xMax, yMax);
			}

			uint32 count() const
			{
				return this->m_PointEncoder.count() / 4;
			}
	};


	template<class _ZOrder, class _TCoord, class _TEncoder, class _TCompressorParams = CompressorParamsBaseImp>
	class TBaseRectCompressor : public TBaseValueDiffEncoder<_ZOrder, _ZOrder, _TEncoder, _TCompressorParams>
	{
		public:
			typedef TBaseValueDiffEncoder<_ZOrder, _ZOrder, _TEncoder, _TCompressorParams> TBase;

			typedef _ZOrder ZOrder;
			typedef _TCoord TCoord;

			TBaseRectCompressor(uint32 nPageSize, CommonLib::alloc_t* pAlloc = nullptr, _TCompressorParams *pCompParams = nullptr) :
				TBase(nPageSize, pAlloc, pCompParams)
			{}



			virtual void Write(const ZOrder& zOrder, CommonLib::IWriteStream *pStream)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				zOrder.getXY(xMin, yMin, xMax, yMax);

				pStream->write(xMin);
				pStream->write(yMin);
				pStream->write(xMax);
				pStream->write(yMax);
			}

			virtual void Read(ZOrder& zOrder, CommonLib::IReadStream *pStream)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				pStream->read(xMin);
				pStream->read(yMin);
				pStream->read(xMax);
				pStream->read(yMax);
				zOrder.setZOrder(xMin, yMin, xMax, yMax);
			}

			virtual uint32 GetValueSize() const
			{
				return sizeof(TCoord) * 4;
			}
	};
	
}

#endif