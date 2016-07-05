#include "stdafx.h"
#include "OIDCompress.h"
#include <algorithm>
#include "CommonLibrary/PodVector.h"
#include "MathUtils.h"
namespace embDB
{


	OIDCompressor::OIDCompressor(CommonLib::alloc_t *pAlloc, uint32 nPageSize, CompressorParamsBaseImp *pParams , uint32 nError) :
		m_NumLenComp(pParams->m_compressType, pParams->m_nErrorCalc, pParams->m_bCalcOnlineSize), m_DiffComp(pParams->m_compressType, pParams->m_nErrorCalc, pParams->m_bCalcOnlineSize)
	{

	}
	OIDCompressor::~OIDCompressor()
	{

	}
	uint32 OIDCompressor::GetCompressSize() const
	{
		 uint32 nByteDiffComp = m_DiffComp.GetCompressSize();
		 uint32 nByteNumlenComp = m_NumLenComp.GetCompressSize();

		
		return 1 + min(nByteDiffComp, nByteNumlenComp);
	}
	 

	void OIDCompressor::AddDiffSymbol(int64 nDiff)
	{
		m_DiffComp.AddSymbol(nDiff); 
		m_NumLenComp.AddSymbol(nDiff);
	}
	
	void OIDCompressor::AddSymbol(uint32 nSize,  int nIndex, int64 nOID, const embDB::TBPVector<int64>& vecOIDs)
	{
		if(nSize > 1)
		{

			if(nIndex == 0)
			{
				AddDiffSymbol(vecOIDs[nIndex + 1] - nOID); 
			}
			else
			{
				int64 nPrevOID =  vecOIDs[nIndex - 1];
				if(nIndex == nSize - 1)
				{
					AddDiffSymbol(nOID - nPrevOID); 
				}
				else
				{
					int64 nNextOID =  vecOIDs[nIndex + 1];
					int64 nOldSymbol = nNextOID - nPrevOID;

					RemoveDiffSymbol(nOldSymbol);


					AddDiffSymbol(nOID - nPrevOID); 
					AddDiffSymbol(nNextOID - nOID); 
				}
			}
		}

	
	}

	void OIDCompressor::RemoveSymbol(uint32 nSize,  int nIndex, int64 nOID, const embDB::TBPVector<int64>& vecOIDs)
	{
		if(vecOIDs.size() > 1)
		{
			if(nIndex == 0)
			{
				RemoveDiffSymbol(vecOIDs[nIndex + 1] - nOID); 
			}
			else
			{
				
				if(nIndex == vecOIDs.size() - 1)
				{

					RemoveDiffSymbol(nOID - vecOIDs[nIndex - 1]); 
				}
				else
				{
					int64 nPrevOID =  vecOIDs[nIndex - 1];

					int64 nNextOID =  vecOIDs[nIndex + 1];
					int64 nNewSymbol = nNextOID - nPrevOID;

					AddDiffSymbol(nNewSymbol);


					RemoveDiffSymbol(nOID - nPrevOID); 
					RemoveDiffSymbol(nNextOID - nOID); 
				}
			}
		}
	}
	void OIDCompressor::RemoveDiffSymbol(int64 nDiff)
	{
		m_DiffComp.RemoveSymbol(nDiff);
		m_NumLenComp.RemoveSymbol(nDiff);
	}

	bool OIDCompressor::compress( const embDB::TBPVector<int64>& oids, CommonLib::IWriteStream *pStream)
	{
		if(oids.empty())
			return true;

		uint32 nByteDiffComp = m_DiffComp.GetCompressSize();
		uint32 nByteNumlenComp = m_NumLenComp.GetCompressSize();

		if(nByteNumlenComp < nByteDiffComp)
		{
			pStream->write((byte)eCopmressNumLen);
			return m_NumLenComp.compress(oids, pStream);
		}
		else
		{
			pStream->write((byte)eCompressDiff);
			return m_DiffComp.compress(oids, pStream);
		}
		return false;
	}

	 

	bool OIDCompressor::decompress(uint32 nSize, TBPVector<int64>& oids, CommonLib::IReadStream* pStream)
	{
		if(nSize == 0)
			return true;
		 
		byte nCompSchema = pStream->readByte();
		bool bRet = false;
		if(nCompSchema == (byte)eCompressDiff)
		{
			 bRet =  m_DiffComp.decompress(nSize, oids, pStream);

			 if(bRet)
			 {
				 for (uint32 i = 1; i < nSize; ++i )
				 {
					 m_NumLenComp.AddSymbol(oids[i] - oids[i - 1]);
				 }
				 
			 }
		}
		else
		{
			 bRet = m_NumLenComp.decompress(nSize, oids, pStream);
			 if(bRet)
			 {
				 for (uint32 i = 1; i < nSize; ++i )
				 {
					 m_DiffComp.AddSymbol(oids[i] - oids[i - 1]);
				 }
			 }

		}

		return bRet;
	}
	void OIDCompressor::clear()
	{
		m_DiffComp.clear();
		m_NumLenComp.clear();
	}
}