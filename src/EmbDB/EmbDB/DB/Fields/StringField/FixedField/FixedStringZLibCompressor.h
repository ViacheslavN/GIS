#ifndef _EMBEDDED_DATABASE_FIXED_STRING_Z_LIB_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_FIXED_STRING_Z_LIB_COMPRESSOR_H_
#include <map>
#include <vector>
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "CommonLibrary/RangeCoder.h"
#include "CompressorParams.h"
#include "CommonLibrary/String.h"

#include "utils/compress/zlib/ZLibCompressor.h"
#include <algorithm>
#include "utils/alloc/PageAlloc.h"
#include "../../BaseFieldEncoders.h"

namespace embDB
{

 
	class TFixedStringZlibCompressor
	{
	public:
 
		typedef STLAllocator<CommonLib::CString> TValueAlloc;
		typedef std::vector<CommonLib::CString, TValueAlloc> TValueMemSet;

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

		TFixedStringZlibCompressor(uint32 nPageSize, CommonLib::alloc_t *pAlloc, StringFieldCompressorParams *pParams = nullptr):
			m_nStrings(0), m_nPageSize(nPageSize), m_lenCompressor(nPageSize, pAlloc, pParams), /*m_pCurrBloc(0),*/
				m_pAlloc(pAlloc), m_nTranType(eTT_UNDEFINED), m_bMinSplit(true), m_bDirty(false), m_StringCoding(scUTF8), m_nRowSize(0)
		{
			
			
		}

		~TFixedStringZlibCompressor()
		{
			 DeleteBlocs();
		}
		void init(StringFieldCompressorParams *pParams, int nTranType = eTT_UNDEFINED)
		{
			assert(pParams);

			//m_pValueMemset = pVecValues;
			m_nTranType = nTranType;
			m_StringCoding = pParams->GetStringCoding();
			m_nMaxRowCoeff = pParams->m_nMaxRowCoeff;
			//m_pPageAlloc = pPageAlloc;
		}


		void AddSymbol(uint32 nSize,  int nIndex, const CommonLib::CString& string, const TValueMemSet& vecValues)
		{
			AddLen(nSize, nIndex, string, vecValues);
			AddString(nIndex, string, vecValues);
			m_nRowSize += string.length();
		}

		void AddLen(uint32 nSize,  int nIndex, const CommonLib::CString& string, const TValueMemSet& vecValues)
		{
			if(nSize > 1)
			{
				if(nIndex == 0)
				{
					AddDiffSymbol(vecValues[nIndex + 1].length() - string.length());
				}
				else
				{
					const CommonLib::CString& sPrev =  vecValues[nIndex - 1];
					if(nIndex == nSize - 1)
					{
						AddDiffSymbol(string.length() - sPrev.length());
					}
					else
					{
						const CommonLib::CString& sNext =  vecValues[nIndex + 1];
						int32 nOldLen = sNext.length() - sPrev.length();

						RemoveDiffSymbol(nOldLen);


						AddDiffSymbol(string.length() - sPrev.length());
						AddDiffSymbol(sNext.length() - string.length());
					}
				}
			}

		}

		void RemoveSymbol(uint32 nSize,  int nIndex, const CommonLib::CString& sString, const TValueMemSet& vecValues)
		{
			m_bDirty = true;

			//m_pAlloc->free(nValue.m_pBuf);
			RemoveLen(nSize, nIndex, sString, vecValues);
			RemoveString(nIndex, sString);
			m_nRowSize -= sString.length();
		}

		void UpdateSymbol(uint32 nIndex, CommonLib::CString& newString, const CommonLib::CString& OldValue, const TValueMemSet& vecValues)
		{
			RemoveSymbol(vecValues.size(), nIndex, OldValue, vecValues);
			AddSymbol(vecValues.size(), nIndex, newString, vecValues);
		}

		void RemoveLen(uint32 nSize,  int nIndex, const CommonLib::CString& string, const TValueMemSet& vecValues)
		{
			if(vecValues.size() > 1)
			{
				if(nIndex == 0)
				{
					RemoveDiffSymbol(vecValues[nIndex + 1].length() - string.length());
				}
				else
				{

					if(nIndex == vecValues.size() - 1)
					{

						RemoveDiffSymbol(string.length() - vecValues[nIndex - 1].length());
					}
					else
					{
						const CommonLib::CString& sPrev =  vecValues[nIndex - 1];

						const CommonLib::CString& sNext =  vecValues[nIndex + 1];
						int32 nNewSymbol = sNext.length() - sPrev.length();

						AddDiffSymbol(nNewSymbol);


						RemoveDiffSymbol(string.length() - sPrev.length());
						RemoveDiffSymbol(sNext.length() - string.length());
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

		void AddString(uint32 nIndex, const CommonLib::CString& string, const TValueMemSet& vecValues)
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
				pCurrBloc->m_nRowSize += string.length();		
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


		void SplitBloc(size_t nBlocIDx, sStringBloc *pBloc, uint32 nIndex, const CommonLib::CString& string, const TValueMemSet& vecValues)
		{
			sStringBloc *pNewBloc = new sStringBloc(m_pAlloc);
			m_vecStringBloc.insert(m_vecStringBloc.begin() + nBlocIDx + 1, pNewBloc);
			pNewBloc->m_bDirty = true;
			if(pBloc->m_nBeginIndex + pBloc->m_nCount <= nIndex)
			{
				CompressBlock(pBloc, vecValues);
				pNewBloc->m_nBeginIndex = pBloc->m_nBeginIndex + pBloc->m_nCount;
				pNewBloc->m_nCount += 1;
				pNewBloc->m_nRowSize += string.length();		
				
			}
			else
			{
				int32 nSplitIndex = 0;
				uint32 nRowSize = pBloc->m_nRowSize;
				for (uint32 nSplitIndex = pBloc->m_nBeginIndex + pBloc->m_nCount - 1; nSplitIndex >= pBloc->m_nBeginIndex; --nSplitIndex )
				{
					nRowSize -= vecValues[nSplitIndex].length();
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
			
			std::vector<byte> vecAlloc;

			if(pBloc->m_nCount != 0 &&  pBloc->m_bDirty)
			{
				embDB::CZlibCompressor compressor;
				compressor.BeginEncode(&pBloc->m_compressBlocStream);
				pBloc->m_compressBlocStream.seek(0, CommonLib::soFromBegin);

				for (size_t i = 0; i < pBloc->m_nCount; ++i)
				{
					const CommonLib::CString& string = vecValues[i + pBloc->m_nBeginIndex];
					size_t nSize = 0;

					if (m_StringCoding == embDB::scASCII)
					{
						nSize = string.length() + 1;
						if (vecAlloc.size() < nSize)
							vecAlloc.resize(nSize);

						strcpy((char*)&vecAlloc[0], string.cstr());
						vecAlloc[string.length()] = 0;
					}
					else if (m_StringCoding  == embDB::scUTF8)
					{
						nSize = string.calcUTF8Length() + 1;
						if (vecAlloc.size() < nSize)
							vecAlloc.resize(nSize);
						string.exportToUTF8((char*)&vecAlloc[0], nSize);
					}
					
					compressor.compress(&vecAlloc[0], nSize, &pBloc->m_compressBlocStream);
				}
				compressor.EndEncode(&pBloc->m_compressBlocStream);
				//compressor.compress(&vecValues[0] + pBloc->m_nBeginIndex,pBloc->m_nCount,  &pBloc->m_compressBlocStream);
				pBloc->m_nCompressSize = pBloc->m_compressBlocStream.pos();
			    pBloc->m_bDirty = false;
			}

		}


	 

		void RemoveString(int nIndex, const CommonLib::CString& string)
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
			assert(pCurrBloc->m_nRowSize >= string.length());

			pCurrBloc->m_bDirty = true;
			pCurrBloc->m_nRowSize -= string.length();
			pCurrBloc->m_nCount--;
			m_nStrings--;



			for (size_t i = nBlocIDx + 1; i < m_vecStringBloc.size(); ++i)
			{
				m_vecStringBloc[i]->m_nBeginIndex--;
			}
			 

			//m_pAlloc->free(string.m_pBuf);
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

			/*uint32 nStringSize = 0;
			for (size_t i = 0, sz = m_vecStringBloc.size(); i < sz; ++i)
			{
				nStringSize += m_vecStringBloc[i]->m_nRowSize + sizeof(uint16);
			}

			return nSize + nStringSize + sizeof(uint16) + sizeof(uint16);*/ //begin len begin compress string block

			return nSize + m_nRowSize + sizeof(uint16) + sizeof(uint16);
		}
		void Free()
		{
			/*for (uint32 i = 0; i < m_pValueMemset->size(); ++i )
			{
				sFixedStringVal& val = (*m_pValueMemset)[i];
				m_pAlloc->free(val.m_pBuf);
			}*/
		}
		void clear()
		{
			 DeleteBlocs();
			 m_nStrings = 0;
			 m_lenCompressor.clear();
			 m_nRowSize = 0;
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

		bool encode(const TValueMemSet& vecValues, CommonLib::IWriteStream* pStream)
		{			
					 
			uint32 nBeginPos = pStream->pos();

			pStream->write(uint16(0));
			pStream->write(uint16(vecValues[0].length()));

			if (!m_lenCompressor.BeginEncoding(pStream))
				return false;
			
			for (size_t i = 1; i < vecValues.size(); ++i)
			{
				int32 nLen1 = (int32)vecValues[i].length();
				int32 nLen2 = (int32)vecValues[i -1].length();
				if (!m_lenCompressor.encodeSymbol(nLen1 - nLen2))
					return false;
			}
			if (!m_lenCompressor.FinishEncoding(pStream))
				return false;

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



		bool decode(uint32 nSize, TValueMemSet& vecValues, CommonLib::FxMemoryReadStream *pStream)
		{

			CommonLib::FxMemoryReadStream stringCompressStream;
			short nCompressStringBlock = pStream->readintu16();
			int32 nLen = pStream->readintu16();


			stringCompressStream.attach(pStream, nCompressStringBlock, pStream->size() - nCompressStringBlock);

			m_lenCompressor.BeginDecoding(pStream);
			uint32 nBeginBloc = 0;
			CommonLib::CWriteMemoryStream decodeStream;
			CommonLib::FxMemoryReadStream  stringStream;

			uint32 nDecodeSize = 0;
			int32 nNextLen = 0;

			CZlibCompressor compressor;
			bool bNewBloc = true;
			sStringBloc *pCurrBloc = NULL;

			std::vector<byte> vecAlloc;
			for (uint32 i = 0; i < nSize; ++i)
			{
		
				if(i != 0)
					m_lenCompressor.decodeSymbol(nNextLen);
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

				CommonLib::CString string;


				if (vecAlloc.size() < nLen + 1) //TO DO use alloc
					vecAlloc.resize(nLen + 1);

				pCurrBloc->m_nRowSize += (nLen - 1);
			 
				bNewBloc = !compressor.decompressSymbol(&vecAlloc[0], nLen - 1);

				vecAlloc[nLen - 1] = '\0';

				if (m_StringCoding == embDB::scASCII)
					string.loadFromASCII((const char*)&vecAlloc[0]);
				else if (m_StringCoding == embDB::scUTF8)
					string.loadFromUTF8((const char*)&vecAlloc[0]);
					
	
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

		
		uint32 GetSplitIndex(uint32 nFreePage, const TValueMemSet& vevStrings) const
		{
			uint32 nSplitIndex = 0;
			uint32 nSumSize = 0;
			if(m_vecStringBloc.size() == 1)
			{

				for (size_t i = 0, sz = vevStrings.size(); i< sz; ++i)
				{
					nSumSize += vevStrings[i].length();
					if(nSumSize >= nFreePage)
						return (uint32)i;
				}
				return vevStrings.size() - 1;
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



		void AddBloc(sStringBloc* pBloc, const TValueMemSet& vecStrings, bool bRecalcRowSize = false)
		{

			if(bRecalcRowSize)
			{
				RecalcBloc(pBloc, vecStrings);
			}

			m_vecStringBloc.push_back(pBloc);
			m_nStrings += pBloc->m_nCount;
		}


		void RecalcBloc(sStringBloc* pBloc, const TValueMemSet& vecStrings)
		{

			pBloc->m_bDirty = true;
			pBloc->m_nCompressSize = 0;
			pBloc->m_nRowSize = 0;
			for(size_t i = 0; i < pBloc->m_nCount; ++i)
			{
				pBloc->m_nRowSize += vecStrings[i].length();
			}
		}

		/*void SplitIn(uint32 nBegin, uint32 nEnd, TFixedStringZlibCompressor *pCompressor)
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
		


		}*/
		 
		/*void recalcLen()
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
		*/
		void PreSave(const TValueMemSet& vecStrings)
		{
			if(m_bDirty)
			{

				clear();

				for (uint32 i = 0, sz = vecStrings.size(); i < sz; ++i)
				{
					AddSymbol(i + 1, i, vecStrings[i], vecStrings);
				}
				m_bDirty = false;
			}
		
			for (size_t i = 0, sz = m_vecStringBloc.size(); i < sz; ++i)
			{
				CompressBlock(m_vecStringBloc[i], vecStrings);
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
		SignedNumLenEncoder32   m_lenCompressor;
		

		

		std::vector<sStringBloc*> m_vecStringBloc;
		CommonLib::alloc_t* m_pAlloc;
		//CommonLib::alloc_t* m_pPageAlloc;
		//TValueMemSet* m_pValueMemset;
		int m_nTranType;
		eStringCoding m_StringCoding;
		short m_nMaxRowCoeff;
		bool m_bDirty;
		uint32 m_nRowSize;
 

	};

}
#endif