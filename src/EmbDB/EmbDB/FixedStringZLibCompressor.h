#ifndef _EMBEDDED_DATABASE_FIXED_STRING_Z_LIB_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_FIXED_STRING_Z_LIB_COMPRESSOR_H_
#include <map>
#include <vector>
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "BPVector.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "CommonLibrary/RangeCoder.h"
#include "NumLenCompress.h"
#include "CompressorParams.h"
#include "SignedNumLenDiffCompressor2.h"
#include "StringVal.h"
#include "ZLibCompressor.h"

namespace embDB
{

 
	class TFixedStringZlibCompressor
	{
	public:

		static const uint32 ___nNullTerminatedSymbol = 256;
 
		typedef embDB::TBPVector<sFixedStringVal> TValueMemSet;

		typedef CommonLib::TRangeEncoder<uint64, 64> TRangeEncoder;
		typedef CommonLib::TRangeDecoder<uint64, 64> TRangeDecoder;

		typedef CommonLib::TACEncoder<uint64, 32> TACEncoder;
		typedef CommonLib::TACDecoder<uint64, 32> TACDecoder;

		enum eTypeFreq
		{
			etfByte = 0,
			etfShort = 1,
			etfInt32 = 2
		};

		TFixedStringZlibCompressor(CommonLib::alloc_t *pAlloc, uint32 nPageSize,
			CompressorParamsBaseImp *pParams, uint32 nError = 200 ): 
			m_nStrings(0), m_nPageSize(nPageSize), m_lenCompressor(nError), m_pCurrBloc(0), m_pAlloc(pAlloc),m_pValueMemset(0)
		{


		}

		~TFixedStringZlibCompressor()
		{
			 DeleteBlocs();
		}
		void init(TValueMemSet* pVecValues)
		{
			m_pValueMemset = pVecValues;
		}


		void AddSymbol(uint32 nSize,  int nIndex, const sFixedStringVal& nValue, const TValueMemSet& vecValues)
		{
			if(nSize > 1)
			{
				if(nIndex == 0)
				{
					AddDiffSymbol(vecValues[nIndex + 1].m_nLen - nValue.m_nLen); 
				}
				else
				{
					const sFixedStringVal& nPrev =  vecValues[nIndex - 1];
					if(nIndex == nSize - 1)
					{
						AddDiffSymbol(nValue.m_nLen - nPrev.m_nLen); 
					}
					else
					{
						const sFixedStringVal& nNext =  vecValues[nIndex + 1];
						int32 nOldLen = nNext.m_nLen - nPrev.m_nLen;

						RemoveDiffSymbol(nOldLen);


						AddDiffSymbol(nValue.m_nLen - nPrev.m_nLen); 
						AddDiffSymbol(nNext.m_nLen - nValue.m_nLen); 
					}
				}
			}

			AddString(nValue, vecValues);
		}


		void RemoveSymbol(uint32 nSize,  int nIndex, const sFixedStringVal& nValue, const TValueMemSet& vecValues)
		{
			if(vecValues.size() > 1)
			{
				if(nIndex == 0)
				{
					RemoveDiffSymbol(vecValues[nIndex + 1].m_nLen - nValue.m_nLen); 
				}
				else
				{

					if(nIndex == vecValues.size() - 1)
					{

						RemoveDiffSymbol(nValue.m_nLen - vecValues[nIndex - 1].m_nLen); 
					}
					else
					{
						const sFixedStringVal& nPrev =  vecValues[nIndex - 1];

						const sFixedStringVal& nNext =  vecValues[nIndex + 1];
						int32 nNewSymbol = nNext.m_nLen - nPrev.m_nLen;

						AddDiffSymbol(nNewSymbol);


						RemoveDiffSymbol(nValue.m_nLen - nPrev.m_nLen); 
						RemoveDiffSymbol(nNext.m_nLen - nValue.m_nLen); 
					}
				}
			}

			RemoveString(nIndex, nValue);
		}
		void RemoveDiffSymbol(int32 nLen)
		{
			 m_lenCompressor.RemoveSymbol(nLen);
		}
		void AddDiffSymbol(int32 nLen)
		{
			  m_lenCompressor.AddSymbol(nLen);
		}

		void AddString(const sFixedStringVal& string, const TValueMemSet& vecValues)
		{
			if(!m_pCurrBloc)
			{
				m_vecStringBloc.push_back(new sStringBloc(m_pAlloc));
				m_pCurrBloc = m_vecStringBloc[0];
				m_pCurrBloc->m_nBeginIndex = 0;
			}
			
			m_pCurrBloc->m_nCount += 1;
			m_pCurrBloc->m_nRowSize += string.m_nLen;		

			if(m_pCurrBloc->m_nRowSize > m_nPageSize - 100)
			{
				CompressBlock(vecValues);
			}

			
			m_nStrings++;
	
		}

		void CompressBlock(const TValueMemSet& vecValues)
		{
			embDB::CZlibCompressor compressor;
			if(m_pCurrBloc->m_nCount != 0)
			{
				compressor.compress(&vecValues[0] + m_pCurrBloc->m_nBeginIndex,m_pCurrBloc->m_nCount,  &m_pCurrBloc->m_compressBlocStream);
				m_pCurrBloc->m_nCompressSize = m_pCurrBloc->m_compressBlocStream.pos();

				uint32 nBegin = m_pCurrBloc->m_nBeginIndex + m_pCurrBloc->m_nCount;

				m_vecStringBloc.push_back(new sStringBloc(m_pAlloc));
				m_pCurrBloc = m_vecStringBloc.back();
				m_pCurrBloc->m_nBeginIndex = nBegin;
			}
			
		}


		void RemoveString(int nIndex, const sFixedStringVal& string)
		{

		}

	
		uint32 GetComressSize() const
		{
		   uint32 nSize = m_lenCompressor.GetCompressSize();
		   uint32 nStringSize = GetStringCompressSize();
			
			return nSize + nStringSize + sizeof(uint16) + sizeof(uint16); //begin len begin compress string block
		}
		void Free()
		{
			for (uint32 i = 0; i < m_pValueMemset->size(); ++i )
			{
				sFixedStringVal& val = (*m_pValueMemset)[i];
				m_pAlloc->free(val.m_pBuf);
			}
		}
		void clear()
		{
			 DeleteBlocs();
			 m_nStrings = 0;
			 m_lenCompressor.clear();
		}
		uint32 GetStringCompressSize() const
		{
			uint32 nSize = 0;
			for (size_t i = 0, sz = m_vecStringBloc.size(); i < sz; ++i)
			{
				//if(m_vecStringBloc[i]->m_nCompressSize != 0)
					nSize += m_vecStringBloc[i]->m_nCompressSize + sizeof(uint16);

			}
			
			return nSize;
		}

		bool compress(const TValueMemSet& vecValues, CommonLib::IWriteStream* pStream)
		{			

		 
			uint32 nBeginPos = pStream->pos();

			pStream->write(uint16(0));
			pStream->write(uint16(vecValues[0].m_nLen));

			m_lenCompressor.BeginEncode(pStream);
			
			for (size_t i = 1; i < vecValues.size(); ++i)
			{
				int32 nLen1 = vecValues[i].m_nLen;
				int32 nLen2 = vecValues[i -1].m_nLen;
				m_lenCompressor.EncodeSymbol(vecValues[i].m_nLen - vecValues[i -1].m_nLen, i -1);	
			}
			m_lenCompressor.EncodeFinish();

			uint32 nPos = pStream->pos();

			pStream->seek(nBeginPos, CommonLib::soFromBegin);
			pStream->write((uint16)nPos);
			pStream->seek(nPos, CommonLib::soFromBegin);
			for (size_t i = 0, sz = m_vecStringBloc.size(); i < sz; ++i)
			{

				sStringBloc *pBloc = m_vecStringBloc[i];
				pStream->write((uint16)pBloc->m_compressBlocStream.pos());
				pStream->write(pBloc->m_compressBlocStream.buffer(), pBloc->m_compressBlocStream.pos());

			}
			return true;
		}
		/*bool decompress(uint32 nSize, TValueMemSet& vecValues, CommonLib::FxMemoryReadStream *pStream)
		{

			CommonLib::FxMemoryReadStream stringCompressStream;
			short nCompressStringBlock = pStream->readintu16();
			int32 nLen = pStream->readintu16();


			stringCompressStream.attach(pStream, nCompressStringBlock, pStream->size() - nCompressStringBlock);

			m_lenCompressor.BeginDecode(pStream);
			uint32 nBeginBloc = 0;
			CommonLib::CWriteMemoryStream decodeStream;
			CommonLib::FxMemoryReadStream  stringStream;

			uint32 nDecodeSize = 0;
			int32 nNextLen = 0;
			for (uint32 i = 0; i < nSize; ++i)
			{

				if(i != 0)
					m_lenCompressor.DecodeSymbol(nNextLen, i - 1);

				nLen += nNextLen;
				
				


				if(!m_pCurrBloc || stringStream.pos() == stringStream.size())
				{
					 decompressBlock(&stringCompressStream, i, &decodeStream);
					 if(m_pCurrBloc)
					 {
						 m_pCurrBloc->m_nCount = i - nBeginBloc;
					 }

					 m_pCurrBloc = m_vecStringBloc.back();
					 nBeginBloc = i;
					 stringStream.attach(&decodeStream, 0, decodeStream.pos());
					 decodeStream.seek(0, CommonLib::soFromBegin);
				}
				
				sFixedStringVal string;
				string.m_nLen = nLen;
				string.m_pBuf = (byte*)m_pAlloc->alloc(nLen);
				stringStream.read(string.m_pBuf, string.m_nLen - 1);
				string.m_pBuf[nLen - 1] = '\0';
				vecValues.push_back(string);

			}
			if(m_pCurrBloc)
			{
				m_pCurrBloc->m_nCount = nSize - nBeginBloc;
			}


			return true;
		}*/

		void decompressBlock(CommonLib::FxMemoryReadStream *pStream, int nIndex, CommonLib::CWriteMemoryStream *pDecodeStream)
		{
			uint16 nSize = pStream->readintu16();

			sStringBloc *pStringBloc = new sStringBloc(m_pAlloc);
			pStringBloc->m_nBeginIndex = nIndex;
			pStringBloc->m_compressBlocStream.write(pStream->buffer() + pStream->pos(), nSize);
			
			pStream->seek(nSize, CommonLib::soFromCurrent);
			
			CZlibCompressor compressor;

			compressor.decompress(pStringBloc->m_compressBlocStream.buffer(), pStringBloc->m_compressBlocStream.pos(), pDecodeStream);
			
			m_vecStringBloc.push_back(pStringBloc);
		}



		bool decompress(uint32 nSize, TValueMemSet& vecValues, CommonLib::FxMemoryReadStream *pStream)
		{

			CommonLib::FxMemoryReadStream stringCompressStream;
			short nCompressStringBlock = pStream->readintu16();
			int32 nLen = pStream->readintu16();


			stringCompressStream.attach(pStream, nCompressStringBlock, pStream->size() - nCompressStringBlock);

			m_lenCompressor.BeginDecode(pStream);
			uint32 nBeginBloc = 0;
			CommonLib::CWriteMemoryStream decodeStream;
			CommonLib::FxMemoryReadStream  stringStream;

			uint32 nDecodeSize = 0;
			int32 nNextLen = 0;

			CZlibCompressor compressor;
			bool bNewBloc = true;
			for (uint32 i = 0; i < nSize; ++i)
			{

				if(i != 0)
					m_lenCompressor.DecodeSymbol(nNextLen, i - 1);

				nLen += nNextLen;




				if(!m_pCurrBloc || bNewBloc)
				{
					bNewBloc = false;
					if(m_pCurrBloc)
					{
						compressor.EndDecode();
					}
					ReadBloc(&stringCompressStream, i, &decodeStream);
					if(m_pCurrBloc)
					{
						m_pCurrBloc->m_nCount = i - nBeginBloc;
					}

					m_pCurrBloc = m_vecStringBloc.back();
					nBeginBloc = i;
					compressor.BeginDecode(m_pCurrBloc->m_compressBlocStream.buffer(), m_pCurrBloc->m_compressBlocStream.pos());
				}

				sFixedStringVal string;
				string.m_nLen = nLen;
				string.m_pBuf = (byte*)m_pAlloc->alloc(nLen);

				m_pCurrBloc->m_nRowSize += (nLen - 1);
			 
				bNewBloc = !compressor.decompressSymbol(string.m_pBuf, string.m_nLen - 1);


				string.m_pBuf[nLen - 1] = '\0';
				vecValues.push_back(string);

			}
			if(m_pCurrBloc)
			{
				m_pCurrBloc->m_nCount = nSize - nBeginBloc;
				compressor.EndDecode();
			}


			return true;
		}
		void ReadBloc(CommonLib::FxMemoryReadStream *pStream, int nIndex, CommonLib::CWriteMemoryStream *pDecodeStream)
		{
			uint16 nSize = pStream->readintu16();

			sStringBloc *pStringBloc = new sStringBloc(m_pAlloc);
			pStringBloc->m_nBeginIndex = nIndex;
			pStringBloc->m_compressBlocStream.write(pStream->buffer() + pStream->pos(), nSize);
			pStringBloc->m_nCompressSize;
			pStream->seek(nSize, CommonLib::soFromCurrent);
			m_vecStringBloc.push_back(pStringBloc);
		}

		void DeleteBlocs()
		{
			for (size_t i = 0, sz = m_vecStringBloc.size(); i < sz; ++i)
			{
				delete m_vecStringBloc[i];
			}
			m_pCurrBloc = NULL;
		}

		
		uint32 GetBeginSplitBlocIndex() const
		{
			 sStringBloc *pBloc = m_pCurrBloc;
			 if(pBloc->m_nCount == 0)
			 {

			 }
		}
		uint32 GetEndSplitBlocIndex() const
		{
			 
		}

 	protected:
		uint32 m_nStrings;
		uint32 m_nPageSize;
		SignedDiffNumLenCompressor232i   m_lenCompressor;
		

		struct sStringBloc
		{
			uint32 m_nRowSize;
			uint32 m_nCompressSize;
			uint32 m_nBeginIndex;
			uint32 m_nCount;
			bool m_bDirty;
 

			CommonLib::CWriteMemoryStream m_compressBlocStream;

			sStringBloc(CommonLib::alloc_t *pAlloc) : m_compressBlocStream(pAlloc),
				m_nRowSize(0), m_nCount(0), m_nCompressSize(0), m_bDirty(false)
			{}
							
		};

		std::vector<sStringBloc*> m_vecStringBloc;


		sStringBloc *m_pCurrBloc;
		CommonLib::alloc_t* m_pAlloc;
		TValueMemSet* m_pValueMemset;

	};

}
#endif