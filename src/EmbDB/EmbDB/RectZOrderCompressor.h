#ifndef _EMBEDDED_DATABASE_RECT_ZORDER_COMPRESOR_H_
#define _EMBEDDED_DATABASE_RECT_ZORDER_COMPRESOR_H_


#include "PointZOrderCompressor.h"


namespace embDB
{
	template<class _ZOrder, 
	class _TCoord, uint32 _nMaxBitsLens>
	class TRectZOrderCompressor : public TBaseZOrderCompressor<_ZOrder,_TCoord, _nMaxBitsLens, 4>
	
	{
		public:


			typedef _ZOrder ZOrder;
			typedef _TCoord TCoord;


			typedef TBaseZOrderCompressor<_ZOrder,_TCoord, _nMaxBitsLens, 4> TBase;


			typedef typename TBase::TEncoder TEncoder;
			typedef typename TBase::TDecoder TDecoder;


			TRectZOrderCompressor(EncoderType nType,  uint32 nError = 200 , bool bOnlineCalcSize = false) :
					TBase(nType, nError, bOnlineCalcSize)
			{
 
			}
			void AddZOrder(const ZOrder& zOrder)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				zOrder.getXY(xMin, yMin, xMax, yMax);
				this->AddSymbol(xMin);
				this->AddSymbol(yMin);
				this->AddSymbol(xMax);
				this->AddSymbol(yMax);
			}

			void RemoveZOrder(const ZOrder& zOrder)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				zOrder.getXY(xMin, yMin, xMax, yMax);
				this->RemoveSymbol(xMin);
				this->RemoveSymbol(yMin);
				this->RemoveSymbol(xMax);
				this->RemoveSymbol(yMax);
			}
		
			void WriteZorder(const ZOrder& zOrder, CommonLib::IWriteStream* pStream)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				zOrder.getXY(xMin, yMin, xMax, yMax);
				pStream->write(xMin);
				pStream->write(yMin);
				pStream->write(xMax);
				pStream->write(yMax);
			}

			void ReadZorder(ZOrder& zOrder, CommonLib::IReadStream* pStream)
			{
				TCoord xMin = 0, yMin = 0, xMax = 0, yMax = 0;
				pStream->read(xMin);
				pStream->read(yMin);
				pStream->read(xMax);
				pStream->read(yMax);

				zOrder.setZOrder(xMin, yMin, xMax, yMax);
			}
			
		virtual bool compressZOrder(TEncoder* pEncoder, CommonLib::FxBitWriteStream *pBitStream, const ZOrder& zOrder, uint32 *FreqPrev)
		{
			TCoord xMin = 0, yMin = 0, xMax  = 0, yMax = 0;
			zOrder.getXY(xMin, yMin, xMax, yMax);

			if(!this->compressCoord(pEncoder, pBitStream, xMin, FreqPrev))
				return false;
			if(!this->compressCoord(pEncoder, pBitStream, yMin, FreqPrev))
				return false;
			if(!this->compressCoord(pEncoder, pBitStream, xMax, FreqPrev))
				return false;
			return this->compressCoord(pEncoder, pBitStream, yMax, FreqPrev);
		}



	 
		virtual void DecompressZOrder(TDecoder* pDecoder, CommonLib::FxBitReadStream *pBitStream, ZOrder& zOrder, uint32 *FreqPrev)
		{
			TCoord xMin = 0, yMin = 0, xMax  = 0, yMax = 0;
			this->decompressCoord(pDecoder, pBitStream, xMin, FreqPrev);
			this->decompressCoord(pDecoder, pBitStream, yMin, FreqPrev);
			this->decompressCoord(pDecoder, pBitStream, xMax, FreqPrev);
			this->decompressCoord(pDecoder, pBitStream, yMax, FreqPrev);
			zOrder.setZOrder(xMin, yMin, xMax, yMax);
		}
	 
	};
	
}

#endif