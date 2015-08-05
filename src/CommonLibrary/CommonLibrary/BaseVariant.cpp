#include "stdafx.h"
#include "BaseVariant.h"
#include "ObjVariant.h"


namespace CommonLib
{
 


	IVariant* CreateBaseVariant(int DataType, alloc_t *pAlloc)
	{
		static simple_alloc_t alloc;
		if(!pAlloc)
			pAlloc = &alloc;
		switch(DataType)
		{
		case CommonLib::dtUInteger8:
			return new (pAlloc->alloc(sizeof(CommonLib::TVarUINT8))) CommonLib::TVarUINT8();
			break;
		case CommonLib::dtInteger8:
			return new (pAlloc->alloc(sizeof(CommonLib::TVarINT8))) CommonLib::TVarINT8();
			break;
		case CommonLib::dtUInteger16:
			return new (pAlloc->alloc(sizeof(CommonLib::TVarUINT16))) CommonLib::TVarUINT16();
			break;
		case CommonLib::dtInteger16:
			return new (pAlloc->alloc(sizeof(CommonLib::TVarINT16))) CommonLib::TVarINT16();
			break;
		case CommonLib::dtUInteger32:
			return new (pAlloc->alloc(sizeof(CommonLib::TVarUINT32))) CommonLib::TVarUINT32();
			break;
		case CommonLib::dtInteger32:
			return new (pAlloc->alloc(sizeof(CommonLib::TVarINT32))) CommonLib::TVarINT32();
			break;
		case CommonLib::dtUInteger64:
			return new (pAlloc->alloc(sizeof(CommonLib::TFVarUINT64))) CommonLib::TFVarUINT64();
			break;
		case CommonLib::dtInteger64:
			return new (pAlloc->alloc(sizeof(CommonLib::TVarINT64))) CommonLib::TVarINT64();
			break;
		case CommonLib::dtOid:
			return new (pAlloc->alloc(sizeof(CommonLib::TVarOID))) CommonLib::TVarOID();
			break;
		case CommonLib::dtFloat:
			return new (pAlloc->alloc(sizeof(CommonLib::TVarFloat))) CommonLib::TVarFloat();
			break;
		case CommonLib::dtDouble:
			return new (pAlloc->alloc(sizeof(CommonLib::TVarDouble))) CommonLib::TVarDouble();
			break;
		case CommonLib::dtString:
				return new (pAlloc->alloc(sizeof(CommonLib::TVarString))) CommonLib::TVarString();
			break;
		/*case CommonLib::dtGeometry:
		case CommonLib::dtAnnotation:
			return new (pAlloc->alloc(sizeof(CommonLib::TVarGeometry))) CommonLib::TVarGeometry();
			break;*/

			
		}


		return NULL;
	}
}