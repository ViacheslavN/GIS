#include "stdafx.h"
#include "OIDCompress.h"
#include <algorithm>
#include "CommonLibrary/PodVector.h"
#include "MathUtils.h"
namespace embDB
{


	OIDCompress::OIDCompress(uint32 nError) : m_NumLenComp(nError), m_DiffComp(nError)
	{

	}
	OIDCompress::~OIDCompress()
	{

	}
	uint32 OIDCompress::GetComressSize() const
	{
		 uint32 nByteDiffComp = m_DiffComp.GetCompressSize();
		 uint32 nByteNumlenComp = m_NumLenComp.GetCompressSize();

		
		return 1 + min(nByteDiffComp, nByteNumlenComp);
	}
	 

	void OIDCompress::AddDiffSymbol(int64 nDiff)
	{
		m_DiffComp.AddSymbol(nDiff); 
		m_NumLenComp.AddSymbol(nDiff);
	}
	
	void OIDCompress::AddSymbol(uint32 nSize,  int nIndex, int64 nOID, const embDB::TBPVector<int64>& vecOIDs)
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
				if(nIndex == (vecOIDs.size() - 1))
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

	void OIDCompress::RemoveSymbol(uint32 nSize,  int nIndex, int64 nOID, const embDB::TBPVector<int64>& vecOIDs)
	{
		if(nSize > 1)
		{
			if(nIndex == 0)
			{
				RemoveDiffSymbol(vecOIDs[nIndex + 1] - nOID); 
			}
			else
			{
				
				if(nIndex == nSize)
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
	void OIDCompress::RemoveDiffSymbol(int64 nDiff)
	{
		m_DiffComp.RemoveSymbol(nDiff);
		m_NumLenComp.RemoveSymbol(nDiff);
	}

	bool OIDCompress::compress( const embDB::TBPVector<int64>& oids, CommonLib::IWriteStream *pStream)
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

	 

	bool OIDCompress::decompress(uint32 nSize, TBPVector<int64>& oids, CommonLib::IReadStream* pStream)
	{
		if(nSize == 0)
			return true;
		byte nCompSchema = pStream->readByte();
		if(nCompSchema == (byte)eCompressDiff)
		{
			 return m_DiffComp.decompress(oids, pStream);
		}
		else
		{
			return m_NumLenComp.decompress(oids, pStream);
		}

		return false;
	}
	void OIDCompress::clear()
	{
		m_DiffComp.clear();
		m_NumLenComp.clear();
	}
}