#ifndef _EMBEDDED_DATABASE__DB_FIELD_INFO_H_
#define _EMBEDDED_DATABASE__DB_FIELD_INFO_H_

#include "CommonLibrary/String.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "Key.h"
#include "CommonLibrary/SpatialKey.h"
#include "IDBField.h"
#include "CommonLibrary/BoundaryBox.h"

namespace embDB
{
	class CStorage;
/*	enum eBaseFieldProp
	{
		FP_IS_NOT_EMPTY = 0x01,
		FP_UNIQUE_FIELD = 0x02,
	};
	enum eFieldType
	{
		FT_VALUE_FIELD = 1,
		FT_COUNTER_VALUE_FIELD,
		FT_INDEX_VALUE_FIELD ,
		FT_MULTI_INDEX_VALUE_FIELD,
		FT_SPATIAL_INDEX_VALUE_FIELD,
		FT_MULTI_SPATIAL_INDEX_VALUE_FIELD
	};*/


	struct sFieldInfo
	{
		sFieldInfo() : 
			m_nFieldType(0)
			,m_nFieldDataType(dtUnknown)
			,m_nBaseFieldProp(0)
			,m_nIndexType(0)
			,m_bPrimeryKey(false)
			,m_bSecondaryKey(false)
			,m_nRefTableID(-1)
			,m_nRefTFieldID(-1)
			,m_nFieldPage(-1)
			,m_nFIPage(-1)
			,m_bCheckCRC32(true)
			,m_nOffsetX(0)
			,m_nOffsetY(0)
			,m_nLenField(0)
		{

		}
		virtual ~sFieldInfo(){}

		CommonLib::CString m_sFieldName;
		CommonLib::CString m_sFieldAlias;
		uint32 m_nFieldType;
		uint32 m_nFieldDataType;
		uint32 m_nBaseFieldProp;
		uint32 m_nIndexType;
		bool m_bPrimeryKey;
		bool m_bSecondaryKey;
		int64 m_nRefTableID;
		int64 m_nRefTFieldID;
		int64 m_nFieldPage;
		int64 m_nFIPage;
		bool m_bCheckCRC32;
		CommonLib::TRect2D64 m_Extent;
		uint64	  m_nOffsetX;
		uint64	  m_nOffsetY;
		uint32   m_nLenField;


		bool Read(CommonLib::FxMemoryReadStream* pStream)
		{
			size_t nlenStr = pStream->readInt32();
			if(nlenStr == 0 || nlenStr > size_t(pStream->size() - pStream->pos()))
				return false;
			std::vector<wchar_t> Namebuf(nlenStr + 1, L'\0');
			pStream->read((byte*)&Namebuf[0], nlenStr * 2);
			m_sFieldName = CommonLib::CString(&Namebuf[0]);


			nlenStr = pStream->readInt32();
			if(nlenStr > size_t(pStream->size() - pStream->pos()))
				return false;
			if(nlenStr != 0)
			{
				std::vector<wchar_t> Aliasbuf(nlenStr + 1, L'\0');
				pStream->read((byte*)&Aliasbuf[0], nlenStr * 2);
				m_sFieldAlias = CommonLib::CString(&Aliasbuf[0]);
			}
				
			m_nFieldType = pStream->readIntu32();
			m_nFieldDataType = pStream->readIntu32();
			m_nBaseFieldProp= pStream->readIntu32();
			m_nIndexType = pStream->readIntu32(); 
			m_bPrimeryKey = pStream->readBool();
			m_bSecondaryKey = pStream->readBool();
			m_nRefTableID = pStream->readInt64();
			m_nRefTFieldID = pStream->readInt64();
			m_nFieldPage =  pStream->readInt64();
			m_bCheckCRC32 = pStream->readBool();
			m_nLenField= pStream->readIntu32(); 
			return true;
		}
		void Write(CommonLib::FxMemoryWriteStream* pStream)
		{
			pStream->write((uint32)m_sFieldName.length());
			pStream->write((byte*)m_sFieldName.cwstr(), m_sFieldName.length() * 2);
			pStream->write((uint32)m_sFieldAlias.length());
			pStream->write((byte*)m_sFieldAlias.cwstr(), m_sFieldAlias.length() * 2);
			pStream->write(m_nFieldType);
			pStream->write(m_nFieldDataType);
			pStream->write(m_nBaseFieldProp);
			pStream->write(m_nIndexType);
			pStream->write(m_bPrimeryKey);
			pStream->write(m_bSecondaryKey);
			pStream->write(m_nRefTableID);
			pStream->write(m_nRefTFieldID);
			pStream->write(m_nFieldPage);
			pStream->write(m_bCheckCRC32);
			pStream->write(m_nLenField);
		}
	};


	struct sSpatialFieldInfo : public sFieldInfo
	{
		sSpatialFieldInfo() :  m_nCoordType(scuUnknown), m_dScaleX(1.), m_dScaleY(1.), m_dOffsetX(0.), m_dOffsetY(0.), m_ShapeType(CommonLib::shape_type_null)
		{

		}
		 virtual ~sSpatialFieldInfo(){}
		//CommonLib::TRect2D64 m_nExtent;


		double m_dScaleX;
		double m_dScaleY;
		double m_dOffsetX;
		double m_dOffsetY;

		eSpatialCoordinatesUnits m_nCoordType;
		CommonLib::eShapeType m_ShapeType;
		CommonLib::bbox m_extent;

		bool Read(CommonLib::FxMemoryReadStream* pStream)
		{
			if(!sFieldInfo::Read(pStream))
				return false;

			m_dScaleX = pStream->readDouble();
			m_dScaleY = pStream->readDouble();
			m_dOffsetX = pStream->readDouble();
			m_dOffsetY = pStream->readDouble();

			m_nCoordType = (eSpatialCoordinatesUnits)pStream->readIntu32();
			m_nIndexType = (eDataTypes)pStream->readIntu32();
			pStream->read(m_extent.xMin);
			pStream->read(m_extent.yMin);
			pStream->read(m_extent.xMax);
			pStream->read(m_extent.yMax);
			

			return true;
		}

		void Write(CommonLib::FxMemoryWriteStream* pStream)
		{
			sFieldInfo::Write(pStream);

			  pStream->write(m_dScaleX);
			  pStream->write(m_dScaleY);
			  pStream->write(m_dOffsetX);
			  pStream->write(m_dOffsetY);

			  pStream->write((uint32)m_nCoordType);
			  pStream->write((uint32)m_nIndexType);

			  pStream->write(m_extent.xMin);
			  pStream->write(m_extent.yMin);
			  pStream->write(m_extent.xMax);
			  pStream->write(m_extent.yMax);
		

		}
		/*template <class TPoint>
		void ReadExtent(CommonLib::FxMemoryReadStream* pStream)
		{
			TPoint xMin, xMax, yMin, yMax;
			pStream->read(xMin);
			pStream->read(yMin);
			pStream->read(xMax);
			pStream->read(yMax);
			m_nExtent.set(xMin, yMin, xMax, yMax);
		}

		template <class TPoint>
		void WriteExtent(CommonLib::FxMemoryWriteStream* pStream)
		{
			pStream->write((TPoint)m_nExtent.m_minX);
			pStream->write((TPoint)m_nExtent.m_minY);
			pStream->write((TPoint)m_nExtent.m_maxX);
			pStream->write((TPoint)m_nExtent.m_maxY);
		}*/

	};

	class IDBIndexHandler;

	class IDBFieldHandler : IField
	{
	public:
		IDBFieldHandler(){}
		~IDBFieldHandler(){}
		virtual sFieldInfo* getFieldInfoType() = 0;
		virtual void setFieldInfoType(sFieldInfo* fi) = 0;
		virtual bool save(int64 nAddr, IDBTransactions *pTran) = 0;
		virtual bool load(int64 nAddr, IDBStorage *pStorage) = 0;
		virtual IValueFiled* getValueField(IDBTransactions* pTransactions, IDBStorage *pStorage) = 0;
		virtual bool release(IValueFiled* pField) = 0;

		virtual void setIndexHandler(IDBIndexHandler *pIndexHandler) = 0;
		virtual IDBIndexHandler * getIndexIndexHandler() = 0;

		virtual bool lock() =0;
		virtual bool unlock() =0;

		virtual bool isCanBeRemoving() = 0;
		 
	};


	class CDBFieldHandlerBase  : IDBFieldHandler
	{
	public:

		CDBFieldHandlerBase(CommonLib::alloc_t* pAlloc) : m_pAlloc(pAlloc), m_pIndexHandler(0)
		{}
		~CDBFieldHandlerBase(){}
		template<class TField>
		bool save(int64 nAddr, IDBTransactions *pTran, CommonLib::alloc_t *pAlloc,  uint16 nObjectPageType, uint16 nSubObjectPageType,
			int64 nInnerCompParams = -1, int64 nLeafCompParams = -1)
		{
			//m_nFieldInfoPage = nAddr;
			FilePagePtr pPage(pTran->getFilePage(nAddr));
			if(!pPage.get())
				return false;
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, nObjectPageType, nSubObjectPageType);
			int64 m_nBTreeRootPage = -1;
			FilePagePtr pRootPage(pTran->getNewPage());
			if(!pRootPage.get())
				return false;
			m_nBTreeRootPage = pRootPage->getAddr();
			stream.write(m_nBTreeRootPage);
			header.writeCRC32(stream);
			pPage->setFlag(eFP_CHANGE, true);
			pTran->saveFilePage(pPage);

			TField field(pTran, pAlloc);
			field.init(m_nBTreeRootPage, nInnerCompParams, nLeafCompParams);
			return field.save();
		}
		bool isCanBeRemoving()
		{
			return true;
		}

		virtual void setIndexHandler(IDBIndexHandler *pIndexHandler)
		{
			m_pIndexHandler = pIndexHandler;
		}
		virtual IDBIndexHandler * getIndexIndexHandler()
		{
			return m_pIndexHandler;
		}
		virtual bool lock()
		{
			return true;
		}
		virtual bool unlock()
		{
			return true;
		}

		eDataTypes getType() const
		{
			return (eDataTypes)m_fi.m_nFieldDataType;
		}
		const CommonLib::CString& getName() const
		{
			return m_fi.m_sFieldName;
		}

		virtual sFieldInfo* getFieldInfoType()
		{
			return &m_fi;
		}
		virtual void setFieldInfoType(sFieldInfo* fi)
		{
			assert(fi);
			m_fi = *fi;
		}
		virtual bool load(int64 nAddr, IDBStorage *pStorage)
		{
			return true;
		}

	protected:
		sFieldInfo m_fi;
		CommonLib::alloc_t* m_pAlloc;
		IDBIndexHandler *m_pIndexHandler;
	};

 
}
#endif