#ifndef _EMBEDDED_DATABASE__DB_FIELD_INFO_H_
#define _EMBEDDED_DATABASE__DB_FIELD_INFO_H_

#include "CommonLibrary/str_t.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "Key.h"
#include "SpatialKey.h"
#include "IDBField.h"

namespace embDB
{
	class CStorage;
	enum eBaseFieldProp
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
	};
	struct sFieldInfo
	{
		sFieldInfo() : 
			m_nFieldType(0)
			,m_nFieldDataType(ftUnknown)
			,m_nBaseFieldProp(0)
			,m_bPrimeryKey(false)
			,m_bSecondaryKey(false)
			,m_nRefTableID(0)
			,m_nRefTFieldID(0)
			,m_nFieldPage(0)
			,m_nFIPage(-1)
			,m_bCheckCRC32(true)
			,m_nOffsetX(0)
			,m_nOffsetY(0)
		{

		}
		CommonLib::str_t m_sFieldName;
		CommonLib::str_t m_sFieldAlias;
		uint32 m_nFieldType;
		uint32 m_nFieldDataType;
		uint32 m_nBaseFieldProp;
		bool m_bPrimeryKey;
		bool m_bSecondaryKey;
		int64 m_nRefTableID;
		int64 m_nRefTFieldID;
		int64 m_nFieldPage;
		int64 m_nFIPage;
		bool m_bCheckCRC32;
		TRect2D64 m_Extent;
		uint64	  m_nOffsetX;
		uint64	  m_nOffsetY;


		bool Read(CommonLib::FxMemoryReadStream* pStream)
		{
			size_t nlenStr = pStream->readInt32();
			if(nlenStr == 0 || nlenStr > size_t(pStream->size() - pStream->pos()))
				return false;
			std::vector<wchar_t> Namebuf(nlenStr + 1, L'\0');
			pStream->read((byte*)&Namebuf[0], nlenStr * 2);
			m_sFieldName = CommonLib::str_t(&Namebuf[0]);


			nlenStr = pStream->readInt32();
			if(nlenStr > size_t(pStream->size() - pStream->pos()))
				return false;
			if(nlenStr != 0)
			{
				std::vector<wchar_t> Aliasbuf(nlenStr + 1, L'\0');
				pStream->read((byte*)&Aliasbuf[0], nlenStr * 2);
				m_sFieldAlias = CommonLib::str_t(&Aliasbuf[0]);
			}
				
			m_nFieldType = pStream->readInt32();
			m_nFieldDataType = pStream->readInt32();
			m_nBaseFieldProp= pStream->readInt32();
			m_bPrimeryKey = pStream->readBool();
			m_bSecondaryKey = pStream->readBool();
			m_nRefTableID = pStream->readInt64();
			m_nRefTFieldID = pStream->readInt64();
			m_nFieldPage =  pStream->readInt64();
			m_bCheckCRC32 = pStream->readBool();
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
			pStream->write(m_bPrimeryKey);
			pStream->write(m_bSecondaryKey);
			pStream->write(m_nRefTableID);
			pStream->write(m_nRefTFieldID);
			pStream->write(m_nFieldPage);
			pStream->write(m_bCheckCRC32);
		}
	};


	struct sSpatialFieldInfo : public sFieldInfo
	{
		sSpatialFieldInfo() : m_nFieldPointType(-1)
		{

		}
		int32 m_nFieldPointType;
		TRect2D64 m_nExtent;

		bool Read(CommonLib::FxMemoryReadStream* pStream)
		{
			if(!sFieldInfo::Read(pStream))
				return false;

			m_nFieldPointType = pStream->readInt32();

			switch(m_nFieldPointType)
			{
				case ptUINT16:
					ReadExtent<int16>(pStream);
					break;
				case ptUINT32:
					ReadExtent<int32>(pStream);
					break;
				case ptUINT64:
					ReadExtent<int64>(pStream);
					break;
			}
			

			return true;
		}

		void Write(CommonLib::FxMemoryWriteStream* pStream)
		{
			sFieldInfo::Write(pStream);
			switch(m_nFieldPointType)
			{
			case ptUINT16:
				WriteExtent<int16>(pStream);
				break;
			case ptUINT32:
				WriteExtent<int32>(pStream);
				break;
			case ptUINT64:
				WriteExtent<int64>(pStream);
				break;
			}

		}
		template <class TPoint>
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
		}

	};


	class IDBFieldHandler : IField
	{
	public:
		IDBFieldHandler(){}
		~IDBFieldHandler(){}
		virtual sFieldInfo* getFieldInfoType() = 0;
		virtual void setFieldInfoType(sFieldInfo& fi) = 0;
		virtual bool save(int64 nAddr, IDBTransactions *pTran) = 0;
		virtual bool load(int64 nAddr, IDBStorage *pStorage) = 0;
		virtual IOIDFiled* getOIDField(IDBTransactions* pTransactions, IDBStorage *pStorage) = 0;
		virtual bool release(IOIDFiled* pField) = 0;

		virtual bool lock() =0;
		virtual bool unlock() =0;

		virtual bool isCanBeRemoving() = 0;
		 
	};
}
#endif