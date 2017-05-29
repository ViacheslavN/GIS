#ifndef _EMBEDDED_DATABASE_SPATIAL_RECT_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_SPATIAL_RECT_COMPRESSOR_H_


#include "BPVector.h"
 
#include "RectZOrderCompressor.h"
#include "CompressorParams.h"
#include "utils/compress/UnsignedIntegerNumLenComp.h"
namespace embDB
{
	template <class _TZorderType, class _TZOrderCompressor>
	class TBaseSpatialCompress
	{
	public:
		typedef _TZorderType TZorderType;
		typedef _TZOrderCompressor TZOrderCompressor;



		TBaseSpatialCompress(CommonLib::alloc_t *pAlloc, uint32 nPageSize, CompressorParamsBaseImp *pParams) :
			m_Compressor(pParams->m_compressType, pParams->m_nErrorCalc, pParams->m_bCalcOnlineSize)
		
		{}
		~TBaseSpatialCompress()
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
		uint32 GetCompressSize() const
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