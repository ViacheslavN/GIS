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
#include "utils/compress/NumLenCompress.h"
#include "CompressorParams.h"
#include "utils/compress/SignedNumLenDiffCompressor2.h"
#include "StringVal.h"
#include "utils/compress/zlib/ZLibCompressor.h"
#include <algorithm>
#include "utils/alloc/PageAlloc.h"
namespace embDB
{

 
	class TFixedStringZlibCompressor
	{
	public:
		typedef embDB::TBPVector<sFixedStringVal> TValueMemSet;

		struct sStringBloc
		{
			uint32 m_nRowSize;
			uint32 m_nCompressSize;
			uint32 m_nBeginIndex;
			uint32 m_nCount;
			bool m_bDirty;


			CommonLib::CWriteMemoryStream m_compressBlocStream;

			sStringBloc(CommonLib::alloc_t *pAlloc = NULL) : m_compressBlocStream(pAlloc),
				m_nRowSize(0), m_nCount(0), m_nCompressSize(0), m_bDirty(false), m_nBeginIndex(0)
			{}

		};

		TFixedStringZlibCompressor(CommonLib::alloc_t *pAlloc, uint32 nPageSize,
			CompressorParamsBaseImp *pParams, uint32 nError = 200): 
			m_nStrings(0), m_nPageSize(nPageSize), m_lenCompressor(ACCoding, nError, false), /*m_pCurrBloc(0),*/
				m_pAlloc(pAlloc),m_pValueMemset(0), m_nTranType(eTT_UNDEFINED), m_bMinSplit(true), m_bDirty(false)
		{
			assert(pParams);
			m_nMaxRowCoeff = pParams->m_nMaxRowCoeff;
		}

		~TFixedStringZlibCompressor()
		{
			 DeleteBlocs();
		}
		void init(TValueMemSet* pVecValues, CommonLib::alloc_t *pPageAlloc, int nTranType = eTT_UNDEFINED)
		{
			m_pValueMemset = pVecValues;
			m_nTranType = nTranType;
			m_pPageAlloc = pPageAlloc;
		}


		void AddSymbol(uint32 nSize,  int nIndex, const sFixedStringVal& nValue, const TValueMemSet& vecValues)
		{
			AddLen(nSize, nIndex, nValue, vecValues);
			AddString(nIndex, nValue, vecValues);
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
			m_bDirty = true;
			m_pPageAlloc->free(nValue.m_pBuf);
			//RemoveLen(nSize, nIndex, nValue, vecValues);
			//RemoveString(nIndex, nValue);
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

		void AddString(uint32 nIndex, const sFixedStringVal& string, const TValueMemSet& vecValues)
		{

			sStringBloc *pCurrBloc = NULL;
			sStringBloc findBlock;
			findBlock.m_nBeginIndex = nIndex;

			std::vector<sStringBloc*>::iterator it = std::lower_bound(m_vecStringBloc.begin(), m_vecStringBloc.end(), &findBlock, BlocPred());
			
			size_t nBlocIDx = 0;
			if(it == m_vecStringBloc.end())
			{
				if(!m_vecStringBloc.empty())
				{
					nBlocIDx = m_vecStringBloc.size() -1;
					pCurrBloc = m_vecStringBloc[nBlocIDx];
				}
				else
				{
					m_vecStringBloc.push_back(new sStringBloc(m_pAlloc));
					pCurrBloc = m_vecStringBloc.back();
					pCurrBloc->m_nBeginIndex = nIndex;
					nBlocIDx = m_vecStringBloc.size() - 1;
				}

			}
			else
			{
				nBlocIDx = it - m_vecStringBloc.begin();
				pCurrBloc = (*it);
			}


			if(pCurrBloc->m_nRowSize > m_nPageSize * m_nMaxRowCoeff)
			{
				SplitBloc(nBlocIDx, pCurrBloc, nIndex, string, vecValues);
			}
			else
			{
				pCurrBloc->m_nCount += 1;
				pCurrBloc->m_nRowSize += string.m_nLen;		
				pCurrBloc->m_bDirty = true;
				if(pCurrBloc->m_nRowSize > m_nPageSize * m_nMaxRowCoeff)
				{
					CompressBlock(pCurrBloc, vecValues);
				}
			}
		
			/*
			if(!m_pCurrBloc || (m_pCurrBloc->m_nCompressSize != 0))
			{
				m_vecStringBloc.push_back(new sStringBloc(m_pAlloc));
				m_pCurrBloc = m_vecStringBloc[0];
				m_pCurrBloc->m_nBeginIndex = 0;
			}
			
		

			if(m_pCurrBloc->m_nRowSize > m_nPageSize - 100)
			{
				CompressBlock(vecValues);
			}
			*/
			
			m_nStrings++;
	
		}


		void SplitBloc(size_t nBlocIDx, sStringBloc *pBloc, uint32 nIndex, const sFixedStringVal& string, const TValueMemSet& vecValues)
		{
			sStringBloc *pNewBloc = new sStringBloc(m_pAlloc);
			m_vecStringBloc.insert(m_vecStringBloc.begin() + nBlocIDx + 1, pNewBloc);
			pNewBloc->m_bDirty = true;
			if(pBloc->m_nBeginIndex + pBloc->m_nCount <= nIndex)
			{
				CompressBlock(pBloc, vecValues);
				pNewBloc->m_nBeginIndex = pBloc->m_nBeginIndex + pBloc->m_nCount;
				pNewBloc->m_nCount += 1;
				pNewBloc->m_nRowSize += string.m_nLen;		
				
			}
			else
			{
				int32 nSplitIndex = 0;
				uint32 nRowSize = pBloc->m_nRowSize;
				for (uint32 nSplitIndex = pBloc->m_nBeginIndex + pBloc->m_nCount - 1; nSplitIndex >= pBloc->m_nBeginIndex; --nSplitIndex )
				{
					nRowSize -= vecValues[nSplitIndex].m_nLen;
					if(nRowSize < m_nPageSize)
						break;
				}

				uint32 nCount =  pBloc->m_nBeginIndex + pBloc->m_nCount - nSplitIndex;
				pBloc->m_bDirty = true;
				pBloc->m_nCount = nSplitIndex - pBloc->m_nBeginIndex;

				pNewBloc->m_nBeginIndex = nSplitIndex;
				pNewBloc->m_nCount = nCount;

				
			}



		}


		void CompressBlock(sStringBloc *pBloc, const TValueMemSet& vecValues)
		{
			
			if(pBloc->m_nCount != 0 &&  pBloc->m_bDirty)
			{
				embDB::CZlibCompressor compressor;
				pBloc->m_compressBlocStream.seek(0, CommonLib::soFromBegin);
				compressor.compress(&vecValues[0] + pBloc->m_nBeginIndex,pBloc->m_nCount,  &pBloc->m_compressBlocStream);
				pBloc->m_nCompressSize = pBloc->m_compressBlocStream.pos();
			    pBloc->m_bDirty = false;
			}

		}


	 

		void RemoveString(int nIndex, const sFixedStringVal& string)
		{
			sStringBloc *pCurrBloc = NULL;
			sStringBloc findBlock;
			findBlock.m_nBeginIndex = nIndex;

			std::vector<sStringBloc*>::iterator it = std::lower_bound(m_vecStringBloc.begin(), m_vecStringBloc.end(), &findBlock, BlocPred());

			size_t nBlocIDx = 0;
			if(it == m_vecStringBloc.end())
			{
				assert(!m_vecStringBloc.empty());
				nBlocIDx = 0;
			}
			else
				nBlocIDx = it - m_vecStringBloc.begin();


			assert(m_nStrings > 0);
			
			pCurrBloc = m_vecStringBloc[nBlocIDx];
			assert(pCurrBloc->m_nCount > 0);
			assert(pCurrBloc->m_nRowSize > 0);
			assert(pCurrBloc->m_nRowSize >= string.m_nLen);

			pCurrBloc->m_bDirty = true;
			pCurrBloc->m_nRowSize -= string.m_nLen;
			pCurrBloc->m_nCount--;
			m_nStrings--;



			for (size_t i = nBlocIDx + 1; i < m_vecStringBloc.size(); ++i)
			{
				m_vecStringBloc[i]->m_nBeginIndex--;
			}
			 

			m_pPageAlloc->free(string.m_pBuf);
			if(m_vecStringBloc.size() == 1 || pCurrBloc->m_nRowSize > m_nPageSize /2)
				return;
		
			assert(pCurrBloc->m_nRowSize > 0);

			sStringBloc *pBlocLeft = NULL;
			sStringBloc *pBlocRight = NULL;
			sStringBloc *pDonorBlock = NULL; 
			uint32 nIndexDonor = -1;

			if(nBlocIDx != 0)
				pBlocLeft = m_vecStringBloc[nBlocIDx - 1];

			if(nBlocIDx != m_vecStringBloc.size() - 1)
				pBlocRight = m_vecStringBloc[nBlocIDx + 1];

			uint32 nLeftRowSize = pBlocLeft ? pBlocLeft->m_nRowSize : 0;
			uint32 nRightRowSize = pBlocRight ? pBlocRight->m_nRowSize : 0;

			if(nRightRowSize > nLeftRowSize)
			{
				pDonorBlock = pBlocRight;
				nIndexDonor = (uint32)nBlocIDx + 1;
			}
			else
			{
				pDonorBlock = pBlocLeft;
				nIndexDonor = (uint32)nBlocIDx - 1;
			}

			if(pDonorBlock->m_nRowSize + pCurrBloc->m_nRowSize < m_nPageSize)
			{
				//union
				if(pCurrBloc->m_nBeginIndex < pDonorBlock->m_nBeginIndex)
				{
					//pCurrBloc->m_nBeginIndex = pDonorBlock->m_nBeginIndex;
					pCurrBloc->m_nCount += pDonorBlock->m_nCount;
					m_vecStringBloc.erase(m_vecStringBloc.begin() + nIndexDonor);
				}
				else
				{
					pDonorBlock->m_nCount += pDonorBlock->m_nCount;
					m_vecStringBloc.erase(m_vecStringBloc.begin() + nBlocIDx);
				}
				

				
			}
			else
			{
				//TO DO Alignment
			}
		}

	
		uint32 GetCompressSize() const
		{
		   uint32 nSize = m_lenCompressor.GetCompressSize();
		   uint32 nStringSize = GetStringCompressSize();
			
			return nSize + nStringSize + sizeof(uint16) + sizeof(uint16); //begin len begin compress string block
		}

		uint32 GetNoComressSize() const
		{
			uint32 nSize =  m_nStrings * sizeof(uint16);

			uint32 nStringSize = 0;
			for (size_t i = 0, sz = m_vecStringBloc.size(); i < sz; ++i)
			{
				nStringSize += m_vecStringBloc[i]->m_nRowSize + sizeof(uint16);
			}

			return nSize + nStringSize + sizeof(uint16) + sizeof(uint16); //begin len begin compress string block
		}
		void Free()
		{
			for (uint32 i = 0; i < m_pValueMemset->size(); ++i )
			{
				sFixedStringVal& val = (*m_pValueMemset)[i];
				m_pPageAlloc->free(val.m_pBuf);
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
				if(!m_vecStringBloc[i]->m_bDirty)
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
				int32 nLen1 = (int32)vecValues[i].m_nLen;
				int32 nLen2 = (int32)vecValues[i -1].m_nLen;
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
				assert(!pBloc->m_bDirty);
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
			pStringBloc->m_nCompressSize = nSize;
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
			sStringBloc *pCurrBloc = NULL;
			for (uint32 i = 0; i < nSize; ++i)
			{
		
				if(i != 0)
					m_lenCompressor.DecodeSymbol(nNextLen, i - 1);
				nLen += nNextLen;
				
				if(!pCurrBloc || bNewBloc)
				{
					bNewBloc = false;
					if(pCurrBloc)
					{
						compressor.EndDecode();
					}
					ReadBloc(&stringCompressStream, i, &decodeStream);
					if(pCurrBloc)
					{
						pCurrBloc->m_nCount = i - nBeginBloc;
					}

					pCurrBloc = m_vecStringBloc.back();
					nBeginBloc = i;
					compressor.BeginDecode(pCurrBloc->m_compressBlocStream.buffer(), pCurrBloc->m_compressBlocStream.pos());
				}

				sFixedStringVal string;
				string.m_nLen = nLen;
				string.m_pBuf = (byte*)m_pPageAlloc->alloc(nLen);

				pCurrBloc->m_nRowSize += (nLen - 1);
			 
				bNewBloc = !compressor.decompressSymbol(string.m_pBuf, string.m_nLen - 1);


				string.m_pBuf[nLen - 1] = '\0';
				vecValues.push_back(string);
				m_nStrings++;
			}
			if(pCurrBloc)
			{
				pCurrBloc->m_nCount = nSize - nBeginBloc;
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
			pStringBloc->m_nCompressSize = nSize;
			pStream->seek(nSize, CommonLib::soFromCurrent);
			m_vecStringBloc.push_back(pStringBloc);
		}

		void DeleteBlocs()
		{
			for (size_t i = 0, sz = m_vecStringBloc.size(); i < sz; ++i)
			{
				delete m_vecStringBloc[i];
			}
			m_vecStringBloc.clear();
		}

		
		uint32 GetSplitIndex(uint32 nFreePage) const
		{
			int nSplitIndex = 0;
			uint32 nSumSize = 0;
			if(m_vecStringBloc.size() == 1)
			{

				for (size_t i = 0, sz = m_pValueMemset->size(); i< sz; ++i)
				{
					nSumSize += (*m_pValueMemset)[i].m_nLen;
					if(nSumSize >= nFreePage)
						return i;
				}
				
			}
			
			nFreePage -= m_lenCompressor.GetCompressSize();
			for(size_t i = 0, sz = m_vecStringBloc.size(); i < sz; ++i)
			{
				 sStringBloc *pBloc = m_vecStringBloc[i];
				 if(!pBloc->m_nCount)
					 continue;

				  nSplitIndex = pBloc->m_nBeginIndex;
				 nSumSize += pBloc->m_nCompressSize != 0 ? pBloc->m_nCompressSize : pBloc->m_nRowSize;
				 if(nSumSize >= nFreePage)
					 break;
				

			}
			 return nSplitIndex;
		}
	

		struct BlocPred
		{

			bool operator() (const sStringBloc* pLeft, const sStringBloc* pRight) const
			{
				return pLeft->m_nBeginIndex < pRight->m_nBeginIndex;
			}
		};



		void AddBloc(sStringBloc* pBloc, bool bRecalcRowSize = false)
		{

			if(bRecalcRowSize)
			{
				RecalcBloc(pBloc);
			}

			m_vecStringBloc.push_back(pBloc);
			m_nStrings += pBloc->m_nCount;
		}


		void RecalcBloc(sStringBloc* pBloc)
		{

			pBloc->m_bDirty = true;
			pBloc->m_nCompressSize = 0;
			pBloc->m_nRowSize = 0;
			for(size_t i = 0; i < pBloc->m_nCount; ++i)
			{
				const sFixedStringVal& val = m_pValueMemset->GetAt(i);
				pBloc->m_nRowSize += val.m_nLen;
			}
		}

		void SplitIn(uint32 nBegin, uint32 nEnd, TFixedStringZlibCompressor *pCompressor)
		{
			sStringBloc findBlock;
			findBlock.m_nBeginIndex = nBegin;
			std::vector<sStringBloc*>::iterator it = std::lower_bound(m_vecStringBloc.begin(), m_vecStringBloc.end(), &findBlock, BlocPred());
			if(it == m_vecStringBloc.end())
				it = m_vecStringBloc.begin();


			sStringBloc* pBloc = *it;
			if(pBloc->m_nBeginIndex != nBegin || m_vecStringBloc.size() == 1)
			{
				int nNewCount = pBloc->m_nCount - nBegin;
			

				sStringBloc* pNewBloc = new sStringBloc(pCompressor->m_pAlloc);
				pNewBloc->m_nBeginIndex = 0;
				pBloc->m_nCount = pBloc->m_nCount - nNewCount;
				pNewBloc->m_nCount = nNewCount;
				pCompressor->AddBloc(pNewBloc, true);
				RecalcBloc(pBloc);

				it++;

			}

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
			if(m_bDirty)
			{

				clear();

				for (uint32 i = 0, sz = m_pValueMemset->size(); i < sz; ++i)
				{
					AddSymbol(i + 1, i, m_pValueMemset->GetAt(i), *m_pValueMemset);
				}
				m_bDirty = false;
			}
		
			for (size_t i = 0, sz = m_vecStringBloc.size(); i < sz; ++i)
			{
					CompressBlock(m_vecStringBloc[i], *m_pValueMemset);
			}
		}

		int GetStringBloc() const
		{
			return m_vecStringBloc.size();
		}
 	protected:
		uint32 m_nStrings;
		uint32 m_nPageSize;
		bool m_bMinSplit;
		SignedDiffNumLenCompressor232i   m_lenCompressor;
		

		

		std::vector<sStringBloc*> m_vecStringBloc;
		CommonLib::alloc_t* m_pAlloc;
		CommonLib::alloc_t* m_pPageAlloc;
		TValueMemSet* m_pValueMemset;
		int m_nTranType;
		short m_nMaxRowCoeff;
		bool m_bDirty;
 

	};

}
#endif