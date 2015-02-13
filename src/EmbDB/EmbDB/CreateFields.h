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
}

#endif