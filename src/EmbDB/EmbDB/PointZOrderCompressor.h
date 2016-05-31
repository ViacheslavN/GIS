#ifndef _EMBEDDED_DATABASE_POINT_ZORDER_COMPRESOR_H_
#define _EMBEDDED_DATABASE_POINT_ZORDER_COMPRESOR_H_

#include "ZOrderCompressBase.h"
//#include "CommonLibrary/FileStream.h"


namespace embDB
{
	template<class _ZOrder, class _TCoord, uint32 _nMaxBitsLens>
	class TPointZOrderCompressor : public TBaseZOrderCompressor<_ZOrder, _TCoord, _nMaxBitsLens, 2>
	
	{
		public:


			typedef _ZOrder ZOrder;
			typedef _TCoord TCoord;



			typedef TBaseZOrderCompressor<_ZOrder, _TCoord, _nMaxBitsLens, 2> TBase;
			typedef typename TBase::TEncoder TEncoder;
			typedef typename TBase::TDecoder TDecoder;

			TPointZOrderCompressor(CompressType nType, uint32 nError = 200 , bool bOnlineCalcSize = false) :
					TBase(nType, nError, bOnlineCalcSize)
			{
 
			}


			virtual void AddZOrder(const ZOrder& zOrder)
			{
				TCoord x = 0, y = 0;
				zOrder.getXY(x, y);
				this->AddSymbol(x);
				this->AddSymbol(y);
			}

			virtual void RemoveZOrder(const ZOrder& zOrder)
			{
				TCoord x = 0, y = 0;
				zOrder.getXY(x, y);
				this->RemoveSymbol(x);
				this->RemoveSymbol(y);
			}
 


			virtual void WriteZorder(const ZOrder& zOrder, CommonLib::IWriteStream* pStream)
			{
				TCoord x = 0, y = 0;
				zOrder.getXY(x, y);
				pStream->write(x);
				pStream->write(y);
			}

			virtual void ReadZorder(ZOrder& zOrder, CommonLib::IReadStream* pStream)
			{
				TCoord x = 0, y = 0;
				pStream->read(x);
				pStream->read(y);

				zOrder.setZOrder(x, y);
			}

			virtual bool compressZOrder(TEncoder* pEncoder, CommonLib::FxBitWriteStream *pBitStream, const ZOrder& zOrder, uint32 *FreqPrev)
			{
				TCoord x = 0, y = 0;
				zOrder.getXY(x, y);

				if(!this->compressCoord(pEncoder, pBitStream, x, FreqPrev))
					return false;
				return this->compressCoord(pEncoder, pBitStream, y, FreqPrev);
			}




			virtual void DecompressZOrder(TDecoder* pDecoder, CommonLib::FxBitReadStream *pBitStream, ZOrder& zOrder, uint32 *FreqPrev)
			{
				TCoord x = 0, y = 0;
				 this->decompressCoord(pDecoder, pBitStream, x, FreqPrev);
				 this->decompressCoord(pDecoder, pBitStream, y, FreqPrev);
				zOrder.setZOrder(x, y);
			}

 
	};
	
}

#endif