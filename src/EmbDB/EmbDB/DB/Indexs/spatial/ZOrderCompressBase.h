#pragma once

#include "../../../Utils/compress/NumLen/BaseNumLenEncoder.h"
#include "../../../Utils/compress/NumLen/UnsignedNumLenEncoder.h"
namespace embDB
{
	template<class _ZOrder, class _TPointType, uint32 _nMaxBitsLens, class _TCompParams = CompressorParamsBaseImp>
	class TBaseZOrderCompressor
	
	{
		public:
			
			typedef _ZOrder ZOrder;
			typedef _TPointType TPointType;
			 

			typedef CommonLib::TACEncoder64 TEncoder;
			typedef CommonLib::TACDecoder64 TDecoder;
			typedef UnsignedNumLenEncoder<TPointType, TEncoder, TDecoder, _TCompParams, _nMaxBitsLens> TPointEncoder;
 

			TBaseZOrderCompressor(uint32 nPageSize, CommonLib::alloc_t* pAlloc = nullptr, _TCompParams *pCompParams = nullptr) : m_PointEncoder(nPageSize, pAlloc, pCompParams)
			{}

			virtual uint32 GetValueSize() const
			{
				return sizeof(ZOrder);
			}

			void clear()
			{
				m_PointEncoder.clear();
			}

			
			uint32 GetCompressSize() const
			{
				return m_PointEncoder.GetCompressSize() + GetValueSize();
			}
		

			bool BeginEncoding(CommonLib::IWriteStream *pStream)
			{
				return m_PointEncoder.BeginEncoding(pStream);
			}
			bool FinishEncoding(CommonLib::IWriteStream *pStream)
			{
				return m_PointEncoder.FinishEncoding(pStream);
			}
			
			void BeginDecoding(CommonLib::IReadStream *pStream)
			{
				m_PointEncoder.BeginDecoding(pStream);
			}

			void FinishDecoding()
			{
				m_PointEncoder.FinishDecoding();
			}

			uint32 count()
			{
				return m_PointEncoder.count();
			}

	protected:
		TPointEncoder m_PointEncoder;
	};
	
}
 