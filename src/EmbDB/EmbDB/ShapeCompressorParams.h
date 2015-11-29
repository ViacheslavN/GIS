#ifndef _EMBEDDED_DATABASE_SHAPE_COMPRESSOR_PARAMS_H_
#define _EMBEDDED_DATABASE_SHAPE_COMPRESSOR_PARAMS_H_

#include "embDBInternal.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/IGeoShape.h"
#include "StreamPageIngo.h"
namespace embDB
{


	class ShapeFieldCompressorParams
	{
	public:

		ShapeFieldCompressorParams() :   m_nMaxPageBlobSize(400), 
			m_nStreamPageInfo(-1), m_dOffsetX(0.), m_dOffsetY(0.), m_dScaleX(1.), m_dScaleY(1.),
				m_CoordTypes(stUnknown), m_ShapeType(CommonLib::shape_type_null)
		{}
		virtual ~ShapeFieldCompressorParams(){}


	 

 
		virtual bool load(CommonLib::IReadStream *pStream, IDBTransaction* pTran)
		{
			 

			m_nMaxPageBlobSize =  pStream->readIntu32();
			m_nStreamPageInfo = pStream->readInt64();
			pStream->read(m_dOffsetX);
			pStream->read(m_dOffsetY);
			pStream->read(m_dScaleX);
			pStream->read(m_dScaleY);
			m_CoordTypes = (embDB::eSpatialType)pStream->readIntu32();
			m_ShapeType = (CommonLib::eShapeType)pStream->readIntu32();
			if(m_nStreamPageInfo != -1)
			{
				m_StreamPageInfo.SetRootPage(m_nStreamPageInfo);
				return m_StreamPageInfo.Load(pTran);
			}
			return true;
		}
		virtual bool save(CommonLib::IWriteStream *pStream, IDBTransaction* pTran)
		{
			if(m_nStreamPageInfo == -1)
			{			 
				FilePagePtr pPage = pTran->getNewPage(MIN_PAGE_SIZE);
				m_StreamPageInfo.SetRootPage(pPage->getAddr());
				m_StreamPageInfo.Save(pTran);
				m_nStreamPageInfo = pPage->getAddr();
			}


		 
			pStream->write((uint32)m_nMaxPageBlobSize);
			pStream->write(m_nStreamPageInfo);
			pStream->write(m_dOffsetX);
			pStream->write(m_dOffsetY);
			pStream->write(m_dScaleX);
			pStream->write(m_dScaleY);
			pStream->write((uint32)m_CoordTypes);
			pStream->write((uint32)m_ShapeType);
			return true;
		}

		uint32 GetMaxPageBlobSize() const {return m_nMaxPageBlobSize;}
		void SetMaxPageBlobSize (uint32 nSize) {m_nMaxPageBlobSize = nSize;}


		void SetOffsetX(double dOffsetX)   { m_dOffsetX = dOffsetX; }
		void SetOffsetY(double dOffsetY)   { m_dOffsetY = dOffsetY; } 
		void SetScaleX(double dScaleX)   { m_dScaleX = dScaleX; } 
		void SetScaleY(double dScaleY)   { m_dScaleY = dScaleY; } 
		void SetCoordType(embDB::eSpatialType CoordTypes)   { m_CoordTypes = CoordTypes; } 
		void SetShapeType(CommonLib::eShapeType ShapeType)   { m_ShapeType = ShapeType; } 
	


		double GetOffsetX() const   { return m_dOffsetX;}
		double GetOffsetY()  const { return m_dOffsetY;} 
		double GetScaleX()  const { return m_dScaleX; } 
		double GetScaleY() const  { return m_dScaleY;} 
		embDB::eSpatialType GetCoordType() const  {return  m_CoordTypes;} 
		CommonLib::eShapeType GetShapeType() const  { return m_ShapeType;} 

		CStreamPageInfo* GetStreamInfo(IDBTransaction *pTran)
		{
			return &m_StreamPageInfo;
		}
		ReadStreamPagePtr GetReadStream(IDBTransaction *pTran, int64 nPage = -1, int32 nPos = -1)
		{
			CStreamPageInfo* pStreamInfo = GetStreamInfo(pTran);
			return pStreamInfo->GetReadStream(pTran, nPage, nPos);

		}
		WriteStreamPagePtr GetWriteStream(IDBTransaction *pTran, int64 nPage = -1, int32 nPos = -1)
		{
			CStreamPageInfo* pStreamInfo = GetStreamInfo(pTran);
			return pStreamInfo->GetWriteStream(pTran, nPage, nPos);

		}


		template<class _Transaction>
		void SaveState(_Transaction * pTransaction)
		{
			if(m_StreamPageInfo.GetRootPage() != -1)
				m_StreamPageInfo.Save(pTransaction);
		}
	private:
 
		uint32 m_nMaxPageBlobSize;
		double m_dOffsetX;
		double m_dOffsetY;
		double m_dScaleX;
		double m_dScaleY;
		embDB::eSpatialType m_CoordTypes;
		CommonLib::eShapeType m_ShapeType;
		int64 m_nStreamPageInfo;
		CStreamPageInfo m_StreamPageInfo;

	};
}




#endif