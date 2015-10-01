#ifndef _EMBEDDED_DATABASE_STRING_INNER_NODE_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_STRING_INNER_NODE_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
#include "BPVectorNoPod.h"
#include "StringCompressorParams.h"
namespace embDB
{

	class BPStringInnerNodeSimpleCompressor  
	{
	public:

	 
		typedef int64 TLink;
		typedef  TBPVectorNoPOD<CommonLib::CString> TKeyMemSet;
		typedef  TBPVector<TLink> TLinkMemSet;
		typedef StringFieldCompressorParams TInnerCompressorParams;

		 

		template<typename _Transactions  >
		static TInnerCompressorParams *LoadCompressorParams(int64 nPage, _Transactions *pTran)
		{
			TInnerCompressorParams *pInnerComp = new TInnerCompressorParams(nPage);
			pInnerComp->read(pTran);
			return pInnerComp;
		}

		BPStringInnerNodeSimpleCompressor(CommonLib::alloc_t *pAlloc = 0, 
			TInnerCompressorParams *pParams = 0) : m_nSize(0), m_pAlloc(pAlloc), m_pInnerComp(pParams)
		{

			assert(m_pInnerComp);
		}
		virtual ~BPStringInnerNodeSimpleCompressor(){}
		virtual bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			keySet.reserve(m_nSize);
			linkSet.reserve(m_nSize);

			uint32 nKeySize =  m_nSize * m_pInnerComp->GetStringLen();
			uint32 nLinkSize =  m_nSize * sizeof(int64);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

 			TLink nlink;
			 CommonLib::CString sString;
 
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{


				sString.importFromUTF8()

				KeyStreams.read(zPoint.m_nZValue[i]);
		
				LinkStreams.read(nlink);

				keySet.push_back(zPoint);
				linkSet.push_back(nlink);
			}
			assert(LinkStreams.pos() < stream.size());
			return true;
		}
		virtual bool Write(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)keySet.size();
			assert(m_nSize == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream LinkStreams;

			uint32 nKeySize =  nSize * TCoordPoint::SizeInByte;
			uint32 nLinkSize =  nSize * sizeof(int64);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);	

			size_t nCount =  TCoordPoint::SizeInByte/8;


			for (size_t i = 0, sz = keySet.size(); i < sz; ++i )
			{

				TCoordPoint& coord = keySet[i];
				for (size_t j = 0; j < nCount; ++j )
				{
					KeyStreams.write(coord.m_nZValue[j]);
				}
				LinkStreams.write(linkSet[i]);
			}
			return true;
		}

		virtual bool insert(const CommonLib::CString& sStr, TLink link )
		{
			m_nSize++;
			uint32 nStrSize = 0;
			switch(m_pInnerComp->GetStringCoding())
			{
				case scASCII:
					nStrSize = sStr.length();
					break;
				case  scUTF8:
					nStrSize = sStr.calcUTF8Length();
					break;
				default:
					assert(false);
					break;;
			}

			m_nRowStringSize += nStrSize;

			assert(link!= 0);
			return true;
		}
		virtual bool add(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			for (size_t i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(keySet[i], linkSet[i]);
			}
			
			return true;
		}
		virtual bool recalc(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nSize = keySet.size();
			return true;
		}
		virtual bool remove(const TCoordPoint& key, TLink link)
		{
			m_nSize--;
			return true;
		}
		virtual bool update(const TCoordPoint& key, TLink link)
		{
			return true;
		}
		virtual size_t size() const
		{
			return (TCoordPoint::SizeInByte + sizeof(TLink) ) *  m_nSize + sizeof(uint32) ;
		}
		virtual size_t count() const
		{
			return m_nSize;
		}
		size_t headSize() const
		{
			return  sizeof(uint32);
		}
		size_t rowSize()
		{
			return (TCoordPoint::SizeInByte+ sizeof(TLink)) *  m_nSize;
		}
		void clear()
		{
			m_nSize = 0;
		}
		size_t tupleSize() const
		{
			return  (TCoordPoint::SizeInByte + sizeof(TLink));
		}
	private:
		size_t m_nRowStringSize;
		size_t m_nSize;
		CommonLib::alloc_t* m_pAlloc;
		TInnerCompressorParams *m_pInnerComp;
	};
}

#endif