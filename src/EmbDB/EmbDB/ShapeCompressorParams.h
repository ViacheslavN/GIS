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

		ShapeFieldCompressorParams(int64 nRootPage = -1) : m_nRootPage(nRootPage), m_nMaxPageBlobSize(400), 
			m_nStreamPageInfo(-1), m_dOffsetX(0.), m_dOffsetY(0.), m_dScaleX(1.), m_dScaleY(1.),
				m_CoordTypes(dtUnknown), m_ShapeType(CommonLib::shape_type_null)
		{}
		virtual ~ShapeFieldCompressorParams(){}


		virtual int64 getRootPage() const 
		{
			return m_nRootPage;
		}
		virtual void setRootPage(int64 nPageID)
		{
			m_nRootPage = nPageID;
		}
		virtual bool read(IDBTransaction *pTran)
		{
			FilePagePtr pPage = pTran->getFilePage(m_nRootPage);
			if(!pPage.get())
				return false; //TO DO Error
			CommonLib::FxMemoryReadStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream);
			if(header.m_nObjectPageType != BTREE_PAGE || header.m_nSubObjectPageType != BTREE_SHAPE_PARAMS_COMPRESS_PAGE)
			{
				pTran->error(_T("BTREE: Page %I64d is not shape params compress page"), m_nRootPage);
				return false;
			}

			m_nMaxPageBlobSize = (eStringCoding)stream.readIntu32();
			m_nStreamPageInfo = stream.readInt64();
			stream.read(m_dOffsetX);
			stream.read(m_dOffsetY);
			stream.read(m_dScaleX);
			stream.read(m_dScaleY);
			m_CoordTypes = (embDB::eDataTypes)stream.readIntu32();
			m_ShapeType = (CommonLib::eShapeType)stream.readIntu32();
			if(m_nStreamPageInfo != -1)
			{
				m_StreamPageInfo.SetRootPage(m_nStreamPageInfo);
				return m_StreamPageInfo.Load(pTran);
			}
			return true;
		}
		virtual bool save(IDBTransaction *pTran)
		{
			FilePagePtr pPage = pTran->getFilePage(m_nRootPage);
			if(!pPage.get())
				return false; //TO DO Error


			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, BTREE_PAGE, BTREE_SHAPE_PARAMS_COMPRESS_PAGE);
			stream.write((uint32)m_nMaxPageBlobSize);
			stream.write(m_nStreamPageInfo);
			stream.write(m_dOffsetX);
			stream.write(m_dOffsetY);
			stream.write(m_dScaleX);
			stream.write(m_dScaleY);
			stream.write((uint32)m_CoordTypes);
			stream.write((uint32)m_ShapeType);

			header.writeCRC32(stream);
			pTran->saveFilePage(pPage);
			if(m_nStreamPageInfo != -1)
			{			 
				return m_StreamPageInfo.Save(pTran);
			}
			return !pTran->isError();
		}

		uint32 GetMaxPageBlobSize() const {return m_nMaxPageBlobSize;}
		void SetMaxPageBlobSize (uint32 nSize) {m_nMaxPageBlobSize = nSize;}


		void SetOffsetX(double dOffsetX)   { m_dOffsetX = dOffsetX; }
		void SetOffsetY(double dOffsetY)   { m_dOffsetY = dOffsetY; } 
		void SetScaleX(double dScaleX)   { m_dScaleX = dScaleX; } 
		void SetScaleY(double dScaleY)   { m_dScaleY = dScaleY; } 
		void SetCoordType(embDB::eDataTypes CoordTypes)   { m_CoordTypes = CoordTypes; } 
		void SetShapeType(CommonLib::eShapeType ShapeType)   { m_ShapeType = ShapeType; } 
		void SetParams(const sSpatialFieldInfo& spInfo)
		{
			SetOffsetX(spInfo.m_dOffsetX);
			SetOffsetY(spInfo.m_dOffsetY);
			SetScaleX(spInfo.m_dScaleX);
			SetScaleY(spInfo.m_dScaleY);
			SetCoordType((embDB::eDataTypes)spInfo.m_nFieldType);
			SetShapeType(spInfo.m_ShapeType);
		}



		double GetOffsetX() const   { return m_dOffsetX;}
		double GetOffsetY()  const { return m_dOffsetY;} 
		double GetScaleX()  const { return m_dScaleX; } 
		double GetScaleY() const  { return m_dScaleY;} 
		embDB::eDataTypes GetCoordType() const  {return  m_CoordTypes;} 
		CommonLib::eShapeType GetShapeType() const  { return m_ShapeType;} 

		CStreamPageInfo* GetStreamInfo(IDBTransaction *pTran)
		{
			if(m_StreamPageInfo.GetRootPage() == -1)
			{
				FilePagePtr pPage = pTran->getNewPage();
				m_StreamPageInfo.SetRootPage(pPage->getAddr());
				m_StreamPageInfo.Init(pTran);
			}
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
		int64 m_nRootPage;
		uint32 m_nMaxPageBlobSize;
		double m_dOffsetX;
		double m_dOffsetY;
		double m_dScaleX;
		double m_dScaleY;
		embDB::eDataTypes m_CoordTypes;
		CommonLib::eShapeType m_ShapeType;
		int64 m_nStreamPageInfo;
		CStreamPageInfo m_StreamPageInfo;

	};
}




#endif