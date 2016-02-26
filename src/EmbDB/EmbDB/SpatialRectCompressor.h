#ifndef _EMBEDDED_DATABASE_SPATIAL_RECT_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_SPATIAL_RECT_COMPRESSOR_H_


#include "BPVector.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "CommonLibrary/RangeCoder.h"
#include "RectZOrderCompressor.h"

namespace embDB
{
	template <class _TZorderType, class _TCoordType>
	class TSpatialRectCompress
	{
	public:
		typedef _TZorderType TZorderType;
		typedef _TCoordType TCoordType;


		typedef CommonLib::TRangeEncoder<uint64, 64> TRangeEncoder;
		typedef CommonLib::TRangeDecoder<uint64, 64> TRangeDecoder;

		typedef CommonLib::TACEncoder<uint64, 32> TACEncoder;
		typedef CommonLib::TACDecoder<uint64, 32> TACDecoder;


		typedef TRectZOrderCompressor<_TCoordType, TRangeEncoder, TACEncoder, 
			TRangeDecoder, TACDecoder, TZorderType, sizeof(TCoordType) * 8> TZOrderCompressor;



		TSpatialRectCompress(){}
		~TSpatialRectCompress()
		{

		}
		void AddSymbol(uint32 nSize,  int nIndex, const TZorderType& zOrder, const embDB::TBPVector<TZorderType>& vecZOrders)
		{
			if(nSize > 1)
			{

				if(nIndex == 0)
				{
					AddDiffSymbol(vecZOrders[nIndex + 1] - zOrder); 
				}
				else
				{
					TZorderType zPrev =  vecZOrders[nIndex - 1];
					if(nIndex == nSize - 1)
					{
						AddDiffSymbol(zOrder - zPrev); 
					}
					else
					{
						TZorderType zNext =  vecZOrders[nIndex + 1];
						TZorderType zOld = zNext - zPrev;

						RemoveDiffSymbol(zOld);


						AddDiffSymbol(zOrder - zPrev); 
						AddDiffSymbol(zNext - zOrder); 
					}
				}
			}
		}
		void RemoveSymbol(uint32 nSize,  int nIndex, const TZorderType& zOrder, const embDB::TBPVector<TZorderType>& vecZOrders)
		{
			if(nSize > 1)
			{
				if(nIndex == 0)
				{
					RemoveDiffSymbol(vecZOrders[nIndex + 1] - zOrder); 
				}
				else
				{

					if(nIndex == nSize)
					{

						RemoveDiffSymbol(zOrder - vecZOrders[nIndex - 1]); 
					}
					else
					{
						TZorderType zPrev =  vecZOrders[nIndex - 1];

						TZorderType zNext =  vecZOrders[nIndex + 1];
						TZorderType zNew = zNext - zPrev;

						AddDiffSymbol(zNew);


						RemoveDiffSymbol(zOrder - zPrev); 
						RemoveDiffSymbol(zNext - zOrder); 
					}
				}
			}
		}
		void RemoveDiffSymbol(const TZorderType& zOrder)
		{
			m_Compressor.RemoveZOrder(zOrder);
		}
		void AddDiffSymbol(const TZorderType& zOrder)
		{
			m_Compressor.AddZOrder(zOrder);
		}
		uint32 GetComressSize() const
		{
			return m_Compressor.GetCompressSize();
		}

		bool compress( const embDB::TBPVector<TZorderType>& vecZOrders, CommonLib::IWriteStream *pStream)
		{
			m_Compressor.compress(vecZOrders, pStream);
			return true;
		}
		bool decompress(uint32 nSize, embDB::TBPVector<TZorderType>& vecZOrders, CommonLib::IReadStream *pStream)
		{
			return  m_Compressor.decompress(vecZOrders, pStream);
		}
		void clear()
		{
			m_Compressor.clear();

		}
		uint32 count() const
		{
			return m_Compressor.count();
		}
	protected:
		TZOrderCompressor m_Compressor;
	};
}

#endif