#ifndef _EMBEDDED_DATABASE_POINT_ZORDER_COMPRESOR_H_
#define _EMBEDDED_DATABASE_POINT_ZORDER_COMPRESOR_H_

#include "../ZOrderCompressBase.h"
//#include "CommonLibrary/FileStream.h"


namespace embDB
{
	template<class _ZOrder, class _TCoord, uint32 _nMaxBitsLens, class _TCompressorParams = CompressorParamsBaseImp>
	class TPointZOrderCompressor : public TBaseZOrderCompressor<_ZOrder, _TCoord, _nMaxBitsLens, _TCompressorParams>
	
	{
		public:
			typedef _ZOrder ZOrder;
			typedef _TCoord TCoord;

			typedef  TBaseZOrderCompressor<_ZOrder, _TCoord, _nMaxBitsLens, _TCompressorParams> TBase;

			TPointZOrderCompressor(uint32 nPageSize, CommonLib::alloc_t* pAlloc = nullptr, _TCompressorParams *pCompParams = nullptr) :
					TBase(nPageSize, pAlloc, pCompParams)
			{}


			void AddSymbol(const ZOrder& zOrder)
			{
				TCoord x = 0, y = 0;
				zOrder.getXY(x, y);
				this->m_PointEncoder.AddSymbol(x);
				this->m_PointEncoder.AddSymbol(y);
			}

			void RemoveSymbol(const ZOrder& zOrder)
			{
				TCoord x = 0, y = 0;
				zOrder.getXY(x, y);
				this->m_PointEncoder.RemoveSymbol(x);
				this->m_PointEncoder.RemoveSymbol(y);
			}
 
			bool encodeSymbol(const ZOrder& zOrder)
			{
				TCoord x = 0, y = 0;
				zOrder.getXY(x, y);
				if (!this->m_PointEncoder.encodeSymbol(x))
					return false;
				return this->m_PointEncoder.encodeSymbol(y);
			}


			void decodeSymbol(ZOrder& zOrder)
			{
				TCoord x = 0, y = 0;
				 
				this->m_PointEncoder.decodeSymbol(x);
				this->m_PointEncoder.decodeSymbol(y);

				zOrder.setZOrder(x, y);
			}
 
	};



	template<class _ZOrder,class  _TCoord, class _TEncoder, class _TCompressorParams = CompressorParamsBaseImp>
	class TBasePointCompressor : public TBaseValueDiffEncoder<_ZOrder, _ZOrder, _TEncoder, _TCompressorParams>
	{
	public:
		typedef TBaseValueDiffEncoder<_ZOrder, _ZOrder, _TEncoder, _TCompressorParams> TBase;

		typedef _ZOrder ZOrder;
		typedef _TCoord TCoord;

		TBasePointCompressor(uint32 nPageSize, CommonLib::alloc_t* pAlloc = nullptr, _TCompressorParams *pCompParams = nullptr) :
			TBase(nPageSize, pAlloc, pCompParams)
		{}

		virtual void Write(const ZOrder& zOrder, CommonLib::IWriteStream *pStream)
		{
			TCoord x = 0, y = 0;
			zOrder.getXY(x, y);

			pStream->write(x);
			pStream->write(y);
		}

		virtual void Read(ZOrder& zOrder, CommonLib::IReadStream *pStream)
		{
			TCoord x = 0, y = 0;

			pStream->read(x);
			pStream->read(y);
			zOrder.setZOrder(x, y);
		}
		virtual uint32 GetValueSize() const
		{
			return sizeof(TCoord) * 2;
		}
	};

	
}

#endif