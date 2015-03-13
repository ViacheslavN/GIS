#ifndef _EMBEDDED_DATABASE_CREATE_FIELDS_H_
#define _EMBEDDED_DATABASE_CREATE_FIELDS_H_

namespace embDB
{

	static IDBFieldHandler* CreateValueField(sFieldInfo& fi, CDatabase* pDB)
	{
		IDBFieldHandler* pField = NULL;
		switch(fi.m_nFieldDataType)
		{
		case ftInteger8:
			pField = new TOIDFieldINT8(pDB->getBTreeAlloc());
			break;
		case ftUInteger8:
			pField = new TOIDFieldUINT8(pDB->getBTreeAlloc());
			break;
		case ftInteger16:
			pField = new TOIDFieldINT16(pDB->getBTreeAlloc());
			break;
		case ftUInteger16:
			pField = new TOIDFieldUINT16(pDB->getBTreeAlloc());
			break;
		case ftInteger32:
			pField = new TOIDFieldINT32(pDB->getBTreeAlloc());
			break;
		case ftUIInteger32:
			pField = new TOIDFieldUINT32(pDB->getBTreeAlloc());
			break;
		case ftInteger64:
			pField = new TOIDFieldINT64(pDB->getBTreeAlloc());
			break;
		case ftUIInteger64:
			pField = new TOIDFieldUINT64(pDB->getBTreeAlloc());
			break;
		case ftFloat:
			pField = new TOIDFieldFloat(pDB->getBTreeAlloc());
			break;
		case ftDouble:
			pField = new TOIDFieldDouble(pDB->getBTreeAlloc());
			break;
		}

		return pField;
	}





	static IDBIndexHandler* CreateMultiIndex(sFieldInfo& fi, CDatabase* pDB)
	{
		IDBIndexHandler* pIndex = NULL;
		switch(fi.m_nFieldDataType)
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
		case ftUIInteger32:
			pIndex = new TMultiIndexUINT32(pDB->getBTreeAlloc());
			break;
		case ftInteger64:
			pIndex = new TMultiIndexNT64(pDB->getBTreeAlloc());
			break;
		case ftUIInteger64:
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