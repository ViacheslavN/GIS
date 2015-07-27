#ifndef _EMBEDDED_DATABASE_CREATE_FIELDS_H_
#define _EMBEDDED_DATABASE_CREATE_FIELDS_H_

namespace embDB
{

	static IDBFieldHandler* CreateValueField(sFieldInfo& fi, CDatabase* pDB)
	{
		IDBFieldHandler* pField = NULL;
		switch(fi.m_nFieldType)
		{
		case CommonLib::dtInteger8:
			pField = (IDBFieldHandler*)new TValFieldINT8(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtUInteger8:
			pField = (IDBFieldHandler*)new TValFieldUINT8(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtInteger16:
			pField = (IDBFieldHandler*)new TValFieldINT16(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtUInteger16:
			pField = (IDBFieldHandler*)new TValFieldUINT16(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtInteger32:
			pField = (IDBFieldHandler*)new TValFieldINT32(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtUInteger32:
			pField = (IDBFieldHandler*)new TValFieldUINT32(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtInteger64:
			pField =(IDBFieldHandler*) new TValFieldINT64(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtUInteger64:
			pField = (IDBFieldHandler*)new TValFieldUINT64(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtFloat:
			pField = (IDBFieldHandler*)new TValFieldFloat(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtDouble:
			pField = (IDBFieldHandler*)new TValFieldDouble(pDB->getBTreeAlloc());
			break;
		}

		return pField;
	}





	static IDBIndexHandler* CreateMultiIndex(sFieldInfo& fi, CDatabase* pDB)
	{
		IDBIndexHandler* pIndex = NULL;
		switch(fi.m_nFieldType)
		{
		case CommonLib::dtInteger8:
			pIndex = new TMultiIndexINT8(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtUInteger8:
			pIndex = new TMultiIndexUINT8(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtInteger16:
			pIndex = new TMultiIndexINT16(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtUInteger16:
			pIndex = new TMultiIndexUINT16(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtInteger32:
			pIndex = new TMultiIndexINT32(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtUInteger32:
			pIndex = new TMultiIndexUINT32(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtInteger64:
			pIndex = new TMultiIndexNT64(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtUInteger64:
			pIndex = new TMultiIndexNT64(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtFloat:
			pIndex = new TMultiIndexFloat(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtDouble:
			pIndex = new TMultiIndexDouble(pDB->getBTreeAlloc());
			break;
		}

		return pIndex;
	}



	static IDBIndexHandler* CreateUniqueIndex(sFieldInfo& fi, CDatabase* pDB)
	{
		IDBIndexHandler* pIndex = NULL;
		switch(fi.m_nFieldType)
		{
		case CommonLib::dtInteger8:
			pIndex = new TUniqueIndexINT8(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtUInteger8:
			pIndex = new TUniqueIndexUINT8(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtInteger16:
			pIndex = new TUniqueIndexINT16(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtUInteger16:
			pIndex = new TUniqueIndexUINT16(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtInteger32:
			pIndex = new TUniqueIndexINT32(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtUInteger32:
			pIndex = new TUniqueIndexUINT32(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtInteger64:
			pIndex = new TUniqueIndexNT64(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtUInteger64:
			pIndex = new TUniqueIndexNT64(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtFloat:
			pIndex = new TUniqueIndexFloat(pDB->getBTreeAlloc());
			break;
		case CommonLib::dtDouble:
			pIndex = new TUniqueIndexDouble(pDB->getBTreeAlloc());
			break;
		}

		return pIndex;
	}
}

#endif