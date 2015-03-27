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
			pField = new TValFieldINT8(pDB->getBTreeAlloc());
			break;
		case ftUInteger8:
			pField = new TValFieldUINT8(pDB->getBTreeAlloc());
			break;
		case ftInteger16:
			pField = new TValFieldINT16(pDB->getBTreeAlloc());
			break;
		case ftUInteger16:
			pField = new TValFieldUINT16(pDB->getBTreeAlloc());
			break;
		case ftInteger32:
			pField = new TValFieldINT32(pDB->getBTreeAlloc());
			break;
		case ftUInteger32:
			pField = new TValFieldUINT32(pDB->getBTreeAlloc());
			break;
		case ftInteger64:
			pField = new TValFieldINT64(pDB->getBTreeAlloc());
			break;
		case ftUInteger64:
			pField = new TValFieldUINT64(pDB->getBTreeAlloc());
			break;
		case ftFloat:
			pField = new TValFieldFloat(pDB->getBTreeAlloc());
			break;
		case ftDouble:
			pField = new TValFieldDouble(pDB->getBTreeAlloc());
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
}

#endif