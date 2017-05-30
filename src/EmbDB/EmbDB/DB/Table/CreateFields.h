#ifndef _EMBEDDED_DATABASE_CREATE_FIELDS_H_
#define _EMBEDDED_DATABASE_CREATE_FIELDS_H_

namespace embDB
{

	static IDBFieldHolderPtr CreateValueField(const SFieldProp* pFieldProp, CDatabase* pDB, int64 nPageAdd)
	{
		IDBFieldHolderPtr pField;
		switch(pFieldProp->m_dataType)
		{
		case dtInteger8:
			pField = (IDBFieldHolder*)new TValFieldINT8(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtUInteger8:
			pField = (IDBFieldHolder*)new TValFieldUINT8(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtInteger16:
			pField = (IDBFieldHolder*)new TValFieldINT16(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtUInteger16:
			pField = (IDBFieldHolder*)new TValFieldUINT16(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtInteger32:
			pField = (IDBFieldHolder*)new TValFieldINT32(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtUInteger32:
			pField = (IDBFieldHolder*)new TValFieldUINT32(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtInteger64:
			pField =(IDBFieldHolder*) new TValFieldINT64(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtUInteger64:
			pField = (IDBFieldHolder*)new TValFieldUINT64(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtFloat:
			pField = (IDBFieldHolder*)new TValFieldFloat(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtDouble:
			pField = (IDBFieldHolder*)new TValFieldDouble(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtString:
			{
				if(pFieldProp->m_nLenField != 0 && pFieldProp->m_nLenField < pFieldProp->m_nPageSize/25) //TO DO FIX
					pField = (IDBFieldHolder*)new FixedStringValueFieldHolder(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
				else
					pField = (IDBFieldHolder*)new StringValueFieldHolder(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			}
		
			break;
		case dtBlob:
			pField = (IDBFieldHolder*)new BlobValueFieldHolder(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;
		case dtGeometry:
			pField = (IDBFieldHolder*)new ShapeValueFieldHolder(pDB->getBTreeAlloc(), pFieldProp, nPageAdd);
			break;

			
		}

		return pField;
	}





	static IDBIndexHolder* CreateMultiIndex(IDBFieldHolder *pField,  CDatabase* pDB, int64 nPageAddr, const SIndexProp &ip)
	{
		IDBIndexHolder* pIndex = NULL;
		switch(pField->getType())
		{
		case dtInteger8:
			pIndex = new TMultiIndexINT8(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtUInteger8:
			pIndex = new TMultiIndexUINT8(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtInteger16:
			pIndex = new TMultiIndexINT16(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtUInteger16:
			pIndex = new TMultiIndexUINT16(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtInteger32:
			pIndex = new TMultiIndexINT32(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtUInteger32:
			pIndex = new TMultiIndexUINT32(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtInteger64:
			pIndex = new TMultiIndexNT64(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtUInteger64:
			pIndex = new TMultiIndexNT64(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtFloat:
			pIndex = new TMultiIndexFloat(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtDouble:
			pIndex = new TMultiIndexDouble(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		}

		return pIndex;
	}



	static IDBIndexHolder* CreateUniqueIndex(IDBFieldHolder *pField, CDatabase* pDB, int64 nPageAddr, const SIndexProp &ip)
	{
		IDBIndexHolder* pIndex = NULL;
		switch(pField->getType())
		{
		case dtInteger8:
			pIndex = new TUniqueIndexINT8(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtUInteger8:
			pIndex = new TUniqueIndexUINT8(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtInteger16:
			pIndex = new TUniqueIndexINT16(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtUInteger16:
			pIndex = new TUniqueIndexUINT16(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtInteger32:
			pIndex = new TUniqueIndexINT32(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtUInteger32:
			pIndex = new TUniqueIndexUINT32(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtInteger64:
			pIndex = new TUniqueIndexNT64(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtUInteger64:
			pIndex = new TUniqueIndexUINT64(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtFloat:
			pIndex = new TUniqueIndexFloat(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		case dtDouble:
			pIndex = new TUniqueIndexDouble(pField, pDB->getBTreeAlloc(), nPageAddr, ip);
			break;
		}

		return pIndex;
	}
}

#endif