#ifndef _EMBEDDED_DATABASE_CREATE_FIELDS_H_
#define _EMBEDDED_DATABASE_CREATE_FIELDS_H_

namespace embDB
{

	static IDBFieldHandler* CreateValueField(sFieldInfo* fi, CDatabase* pDB, IDBTransaction *pTran)
	{
		IDBFieldHandler* pField = NULL;
		switch(fi->m_nFieldType)
		{
		case dtInteger8:
			pField = (IDBFieldHandler*)new TValFieldINT8(pDB->getBTreeAlloc());
			break;
		case dtUInteger8:
			pField = (IDBFieldHandler*)new TValFieldUINT8(pDB->getBTreeAlloc());
			break;
		case dtInteger16:
			pField = (IDBFieldHandler*)new TValFieldINT16(pDB->getBTreeAlloc());
			break;
		case dtUInteger16:
			pField = (IDBFieldHandler*)new TValFieldUINT16(pDB->getBTreeAlloc());
			break;
		case dtInteger32:
			pField = (IDBFieldHandler*)new TValFieldINT32(pDB->getBTreeAlloc());
			break;
		case dtUInteger32:
			pField = (IDBFieldHandler*)new TValFieldUINT32(pDB->getBTreeAlloc());
			break;
		case dtInteger64:
			pField =(IDBFieldHandler*) new TValFieldINT64(pDB->getBTreeAlloc());
			break;
		case dtUInteger64:
			pField = (IDBFieldHandler*)new TValFieldUINT64(pDB->getBTreeAlloc());
			break;
		case dtFloat:
			pField = (IDBFieldHandler*)new TValFieldFloat(pDB->getBTreeAlloc());
			break;
		case dtDouble:
			pField = (IDBFieldHandler*)new TValFieldDouble(pDB->getBTreeAlloc());
			break;
		case dtString:
			{
				if(fi->m_nLenField != 0 && fi->m_nLenField < pDB->getDBStorage()->getPageSize()/25)
					pField = (IDBFieldHandler*)new FixedStringValueFieldHandler(pDB->getBTreeAlloc());
				else
					pField = (IDBFieldHandler*)new StringValueFieldHandler(pDB->getBTreeAlloc());
			}
		
			break;
		case dtBlob:
			pField = (IDBFieldHandler*)new BlobValueFieldHandler(pDB->getBTreeAlloc());
			break;
		case dtPoint16:
		case dtPoint32:
		case dtPoint64:
		case dtShape16:
		case dtShape32:
		case dtShape64:
		case dtRect16:
		case dtRect32:
		case dtRect64:
			pField = (IDBFieldHandler*)new ShapeValueFieldHandler(pDB->getBTreeAlloc());
			break;

			
		}

		return pField;
	}





	static IDBIndexHandler* CreateMultiIndex(sFieldInfo* fi, CDatabase* pDB)
	{
		IDBIndexHandler* pIndex = NULL;
		switch(fi->m_nFieldType)
		{
		case dtInteger8:
			pIndex = new TMultiIndexINT8(pDB->getBTreeAlloc());
			break;
		case dtUInteger8:
			pIndex = new TMultiIndexUINT8(pDB->getBTreeAlloc());
			break;
		case dtInteger16:
			pIndex = new TMultiIndexINT16(pDB->getBTreeAlloc());
			break;
		case dtUInteger16:
			pIndex = new TMultiIndexUINT16(pDB->getBTreeAlloc());
			break;
		case dtInteger32:
			pIndex = new TMultiIndexINT32(pDB->getBTreeAlloc());
			break;
		case dtUInteger32:
			pIndex = new TMultiIndexUINT32(pDB->getBTreeAlloc());
			break;
		case dtInteger64:
			pIndex = new TMultiIndexNT64(pDB->getBTreeAlloc());
			break;
		case dtUInteger64:
			pIndex = new TMultiIndexNT64(pDB->getBTreeAlloc());
			break;
		case dtFloat:
			pIndex = new TMultiIndexFloat(pDB->getBTreeAlloc());
			break;
		case dtDouble:
			pIndex = new TMultiIndexDouble(pDB->getBTreeAlloc());
			break;
		}

		return pIndex;
	}



	static IDBIndexHandler* CreateUniqueIndex(sFieldInfo* fi, CDatabase* pDB)
	{
		IDBIndexHandler* pIndex = NULL;
		switch(fi->m_nFieldType)
		{
		case dtInteger8:
			pIndex = new TUniqueIndexINT8(pDB->getBTreeAlloc());
			break;
		case dtUInteger8:
			pIndex = new TUniqueIndexUINT8(pDB->getBTreeAlloc());
			break;
		case dtInteger16:
			pIndex = new TUniqueIndexINT16(pDB->getBTreeAlloc());
			break;
		case dtUInteger16:
			pIndex = new TUniqueIndexUINT16(pDB->getBTreeAlloc());
			break;
		case dtInteger32:
			pIndex = new TUniqueIndexINT32(pDB->getBTreeAlloc());
			break;
		case dtUInteger32:
			pIndex = new TUniqueIndexUINT32(pDB->getBTreeAlloc());
			break;
		case dtInteger64:
			pIndex = new TUniqueIndexNT64(pDB->getBTreeAlloc());
			break;
		case dtUInteger64:
			pIndex = new TUniqueIndexNT64(pDB->getBTreeAlloc());
			break;
		case dtFloat:
			pIndex = new TUniqueIndexFloat(pDB->getBTreeAlloc());
			break;
		case dtDouble:
			pIndex = new TUniqueIndexDouble(pDB->getBTreeAlloc());
			break;
		}

		return pIndex;
	}
}

#endif