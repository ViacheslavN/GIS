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
		typedef embDB::TBPVector<sFixedStringVal> TValueMemSet;
	/*	static const uint32 ___nNullTerminatedSymbol = 256;
 
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
		};*/

		TFixedStringZlibCompressor(CommonLib::alloc_t *pAlloc, uint32 nPageSize,
			CompressorParamsBaseImp *pParams, uint32 nError = 200 ): 
			m_nStrings(0), m_nPageSize(nPageSize), m_lenCompressor(ACCoding, nError, false), m_pCurrBloc(0),
				m_pAlloc(pAlloc),m_pValueMemset(0), m_nTranType(eTT_UNDEFINED)
		{


		}

		~TFixedStringZlibCompressor()
		{
			 DeleteBlocs();
		}
		void init(TValueMemSet* pVecValues, int nTranType = eTT_UNDEFINED)
		{
			m_pValueMemset = pVecValues;
			m_nTranType = nTranType;
		}


		void AddSymbol(uint32 nSize,  int nIndex, const sFixedStringVal& nValue, const TValueMemSet& vecValues)
		{
			AddLen(nSize, nIndex, nValue, vecValues);
			AddString(nValue, vecValues);
		}

		void AddLen(uint32 nSize,  int nIndex, const sFixedStringVal& nValue, const TValueMemSet& vecValues)
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

		}

		void RemoveSymbol(uint32 nSize,  int nIndex, const sFixedStringVal& nValue, const TValueMemSet& vecValues)
		{
			
			RemoveLen(nSize, nIndex, nValue, vecValues);
			RemoveString(nIndex, nValue);
		}

		void RemoveLen(uint32 nSize,  int nIndex, const sFixedStringVal& nValue, const TValueMemSet& vecValues)
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
			if(!m_pCurrBloc || (m_pCurrBloc->m_nCompressSize != 0))
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
			if(m_nTranType == eTT_SELECT)
			{
				pStringBloc->m_compressBlocStream.attach(pStream, pStream->pos(), nSize);
				pStringBloc->m_compressBlocStream.seek(0, CommonLib::soFromEnd);
			}
			else
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

		
		uint32 GetSplitIndex(uint32 nFreePage) const
		{
			int nSplitIndex = 0;
			uint32 nSumSize = 0;
			nFreePage -= m_lenCompressor.GetCompressSize();
			for(size_t i = 0, sz = m_vecStringBloc.size(); i < sz; ++i)
			{
				 sStringBloc *pBloc = m_vecStringBloc[i];
				 if(!pBloc->m_nCount)
					 continue;

				 nSumSize += pBloc->m_nCompressSize != 0 ? pBloc->m_nCompressSize : pBloc->m_nRowSize;
				 if(nSumSize >= nFreePage)
					 break;
				 nSplitIndex = pBloc->m_nBeginIndex;

			}
			 return nSplitIndex;
		}
		struct sStringBloc;

		struct BlocPred
		{

			bool operator() (const sStringBloc* pLeft, const sStringBloc* pRight) const
			{
				return pLeft->m_nBeginIndex < pRight->m_nBeginIndex;
			}
		};



		void AddBloc(sStringBloc* pBloc)
		{
			m_vecStringBloc.push_back(pBloc);
			m_pCurrBloc = m_vecStringBloc.back();

			m_nStrings += pBloc->m_nCount;
		}

		void SplitIn(uint32 nBegin, uint32 nEnd, TFixedStringZlibCompressor *pCompressor)
		{
			sStringBloc findBlock;
			findBlock.m_nBeginIndex = nBegin;
			std::vector<sStringBloc*>::iterator it = lower_bound(m_vecStringBloc.begin(), m_vecStringBloc.end(), &findBlock, BlocPred());
			assert(it != m_vecStringBloc.end());

			int nCount = m_vecStringBloc.end() -it;

			int nIndex = 0;
 
			for (; it != m_vecStringBloc.end(); ++it)
			{

				sStringBloc* pBloc = *it;
				pBloc->m_nBeginIndex = nIndex;
				nIndex += pBloc->m_nCount;
				pCompressor->AddBloc(pBloc);
			}
			m_vecStringBloc.resize(m_vecStringBloc.size() - nCount);
			m_pCurrBloc = m_vecStringBloc.back();
			recalcLen();
			pCompressor->recalcLen();
		


		}
		 
		void recalcLen()
		{
			m_lenCompressor.clear();
			for (size_t i = 0, sz = m_vecStringBloc.size(); i < sz; ++i)
			{
				sStringBloc* pBloc = m_vecStringBloc[i];

				for (uint32 n = 0; n < pBloc->m_nCount; ++n)
				{
					int nBeginIndex= pBloc->m_nBeginIndex;
					int nNum = nBeginIndex + n;
					AddLen(nNum + 1, nNum, (*m_pValueMemset)[nNum], *m_pValueMemset);
				}
			}
		}

		void PreSave()
		{
			CompressBlock(*m_pValueMemset);
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

			sStringBloc(CommonLib::alloc_t *pAlloc = NULL) : m_compressBlocStream(pAlloc),
				m_nRowSize(0), m_nCount(0), m_nCompressSize(0), m_bDirty(false)
			{}
							
		};

		std::vector<sStringBloc*> m_vecStringBloc;


		sStringBloc *m_pCurrBloc;
		CommonLib::alloc_t* m_pAlloc;
		TValueMemSet* m_pValueMemset;
		int m_nTranType;

	};

}
#endif