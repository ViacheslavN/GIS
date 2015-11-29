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





	static IDBIndexHandler* CreateMultiIndex(IDBFieldHandler *pField,  CDatabase* pDB, int64 nPageAddr, uint32 nPageSize)
	{
		IDBIndexHandler* pIndex = NULL;
		switch(pField->getType())
		{
		case dtInteger8:
			pIndex = new TMultiIndexINT8(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtUInteger8:
			pIndex = new TMultiIndexUINT8(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtInteger16:
			pIndex = new TMultiIndexINT16(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtUInteger16:
			pIndex = new TMultiIndexUINT16(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtInteger32:
			pIndex = new TMultiIndexINT32(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtUInteger32:
			pIndex = new TMultiIndexUINT32(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtInteger64:
			pIndex = new TMultiIndexNT64(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtUInteger64:
			pIndex = new TMultiIndexNT64(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtFloat:
			pIndex = new TMultiIndexFloat(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtDouble:
			pIndex = new TMultiIndexDouble(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		}

		return pIndex;
	}



	static IDBIndexHandler* CreateUniqueIndex(IDBFieldHandler *pField, CDatabase* pDB, int64 nPageAddr, uint32 nPageSize)
	{
		IDBIndexHandler* pIndex = NULL;
		switch(pField->getType())
		{
		case dtInteger8:
			pIndex = new TUniqueIndexINT8(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtUInteger8:
			pIndex = new TUniqueIndexUINT8(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtInteger16:
			pIndex = new TUniqueIndexINT16(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtUInteger16:
			pIndex = new TUniqueIndexUINT16(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtInteger32:
			pIndex = new TUniqueIndexINT32(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtUInteger32:
			pIndex = new TUniqueIndexUINT32(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtInteger64:
			pIndex = new TUniqueIndexNT64(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtUInteger64:
			pIndex = new TUniqueIndexNT64(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtFloat:
			pIndex = new TUniqueIndexFloat(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		case dtDouble:
			pIndex = new TUniqueIndexDouble(pField, pDB->getBTreeAlloc(), nPageAddr, nPageSize);
			break;
		}

		return pIndex;
	}
}

#endif