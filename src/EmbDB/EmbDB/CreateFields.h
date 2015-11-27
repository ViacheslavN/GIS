#ifndef _EMBEDDED_DATABASE_CREATE_FIELDS_H_
#define _EMBEDDED_DATABASE_CREATE_FIELDS_H_

namespace embDB
{

	static IDBFieldHandlerPtr CreateValueField(const SFieldProp* pFieldProp, CDatabase* pDB, int64 nPageAdd)
	{
		IDBFieldHandlerPtr pField;
		switch(pFieldProp->m_dataType)
		{
		case dtInteger8:
			pField = (IDBFieldHandler*)new TValFieldINT8(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtUInteger8:
			pField = (IDBFieldHandler*)new TValFieldUINT8(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtInteger16:
			pField = (IDBFieldHandler*)new TValFieldINT16(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtUInteger16:
			pField = (IDBFieldHandler*)new TValFieldUINT16(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtInteger32:
			pField = (IDBFieldHandler*)new TValFieldINT32(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtUInteger32:
			pField = (IDBFieldHandler*)new TValFieldUINT32(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtInteger64:
			pField =(IDBFieldHandler*) new TValFieldINT64(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtUInteger64:
			pField = (IDBFieldHandler*)new TValFieldUINT64(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtFloat:
			pField = (IDBFieldHandler*)new TValFieldFloat(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtDouble:
			pField = (IDBFieldHandler*)new TValFieldDouble(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtString:
			{
				if(pFieldProp->m_nLenField != 0 && pFieldProp->m_nLenField < pFieldProp->m_nPageSize/25) //TO DO FIX
					pField = (IDBFieldHandler*)new FixedStringValueFieldHandler(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
				else
					pField = (IDBFieldHandler*)new StringValueFieldHandler(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			}
		
			break;
		case dtBlob:
			pField = (IDBFieldHandler*)new BlobValueFieldHandler(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtGeometry:
			pField = (IDBFieldHandler*)new ShapeValueFieldHandler(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;

			
		}

		return pField;
	}





	static IDBIndexHandler* CreateMultiIndex(eDataTypes FieldType, CDatabase* pDB)
	{
		IDBIndexHandler* pIndex = NULL;
		switch(FieldType)
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



	static IDBIndexHandler* CreateUniqueIndex(eDataTypes FieldType, CDatabase* pDB)
	{
		IDBIndexHandler* pIndex = NULL;
		switch(FieldType)
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