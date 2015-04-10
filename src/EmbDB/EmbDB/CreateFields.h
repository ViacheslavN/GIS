#ifndef _EMBEDDED_DATABASE_CREATE_FIELDS_H_
#define _EMBEDDED_DATABASE_CREATE_FIELDS_H_

namespace embDB
{

	static IDBFieldHandler* CreateValueField(sFieldInfo& fi, CDatabase* pDB)
	{
		IDBFieldHandler* pField = NULL;
		switch(fi.m_nFieldType)
		{
		case ftInteger8:
			pField = (IDBFieldHandler*)new TValFieldINT8(pDB->getBTreeAlloc());
			break;
		case ftUInteger8:
			pField = (IDBFieldHandler*)new TValFieldUINT8(pDB->getBTreeAlloc());
			break;
		case ftInteger16:
			pField = (IDBFieldHandler*)new TValFieldINT16(pDB->getBTreeAlloc());
			break;
		case ftUInteger16:
			pField = (IDBFieldHandler*)new TValFieldUINT16(pDB->getBTreeAlloc());
			break;
		case ftInteger32:
			pField = (IDBFieldHandler*)new TValFieldINT32(pDB->getBTreeAlloc());
			break;
		case ftUInteger32:
			pField = (IDBFieldHandler*)new TValFieldUINT32(pDB->getBTreeAlloc());
			break;
		case ftInteger64:
			pField =(IDBFieldHandler*) new TValFieldINT64(pDB->getBTreeAlloc());
			break;
		case ftUInteger64:
			pField = (IDBFieldHandler*)new TValFieldUINT64(pDB->getBTreeAlloc());
			break;
		case ftFloat:
			pField = (IDBFieldHandler*)new TValFieldFloat(pDB->getBTreeAlloc());
			break;
		case ftDouble:
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
		case ftInteger8:
			pIndex = new TMultiIndexINT8(pDB->getBTreeAlloc());
			break;
		case ftUInteger8:
			pIndex = new TMultiIndexUINT8(pDB->getBTreeAlloc());
			break;
		case ftInteger16:
			pIndex = new TMultiIndexINT16(pDB->getBTreeAlloc());
			break;
		case ftUInteger16:
			pIndex = new TMultiIndexUINT16(pDB->getBTreeAlloc());
			break;
		case ftInteger32:
			pIndex = new TMultiIndexINT32(pDB->getBTreeAlloc());
			break;
		case ftUInteger32:
			pIndex = new TMultiIndexUINT32(pDB->getBTreeAlloc());
			break;
		case ftInteger64:
			pIndex = new TMultiIndexNT64(pDB->getBTreeAlloc());
			break;
		case ftUInteger64:
			pIndex = new TMultiIndexNT64(pDB->getBTreeAlloc());
			break;
		case ftFloat:
			pIndex = new TMultiIndexFloat(pDB->getBTreeAlloc());
			break;
		case ftDouble:
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
		case ftInteger8:
			pIndex = new TUniqueIndexINT8(pDB->getBTreeAlloc());
			break;
		case ftUInteger8:
			pIndex = new TUniqueIndexUINT8(pDB->getBTreeAlloc());
			break;
		case ftInteger16:
			pIndex = new TUniqueIndexINT16(pDB->getBTreeAlloc());
			break;
		case ftUInteger16:
			pIndex = new TUniqueIndexUINT16(pDB->getBTreeAlloc());
			break;
		case ftInteger32:
			pIndex = new TUniqueIndexINT32(pDB->getBTreeAlloc());
			break;
		case ftUInteger32:
			pIndex = new TUniqueIndexUINT32(pDB->getBTreeAlloc());
			break;
		case ftInteger64:
			pIndex = new TUniqueIndexNT64(pDB->getBTreeAlloc());
			break;
		case ftUInteger64:
			pIndex = new TUniqueIndexNT64(pDB->getBTreeAlloc());
			break;
		case ftFloat:
			pIndex = new TUniqueIndexFloat(pDB->getBTreeAlloc());
			break;
		case ftDouble:
			pIndex = new TUniqueIndexDouble(pDB->getBTreeAlloc());
			break;
		}

		return pIndex;
	}
}

#endif