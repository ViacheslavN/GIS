#include "stdafx.h"
#include "GeoDatabaseUtils.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace GeoDatabaseUtils
		{


			template<class Type>
			CommonLib::CVariant DigitiFromString(const CommonLib::CString& sData)
			{
				wchar_t *pEnd = NULL;
				Type Value = (Type)wcstol(sData.cwstr(), &pEnd, 10);
				return CommonLib::CVariant(Value);
			}


			CommonLib::CVariant GetVariantFromString(eDataTypes type, const CommonLib::CString& sData)
			{
				wchar_t *pEnd = NULL;
				switch(type)
				{
					case dtInteger8:
						return DigitiFromString<int8>(sData);
						break;
					case dtUInteger8:
						return DigitiFromString<byte>(sData);
						break;
					case dtInteger16:
						return DigitiFromString<int16>(sData);
						break;
					case dtUInteger16:
						return DigitiFromString<uint16>(sData);
						break;
					case dtInteger32:
						return DigitiFromString<int32>(sData);
						break;
					case dtUInteger32:
						return DigitiFromString<uint32>(sData);
						break;
					case dtFloat:
						return DigitiFromString<float>(sData);
						break;
					case dtDouble:
						return DigitiFromString<double>(sData);
						break;
					case dtString:
						{
							return CommonLib::CVariant(sData);
						}
						break;
					case dtInteger64:
						{
							int64 nVal =  
#ifdef _WIN32
								_wcstoi64(sData.cwstr(), &pEnd, 10);
#else
								(uint64)wcstoll(sData.cwstr(), &pEnd, 10);
#endif

							return CommonLib::CVariant(nVal);
						}
						break;
					case dtUInteger64:
						{
							uint64 nVal =  
#ifdef _WIN32
								_wcstoi64(sData.cwstr(), &pEnd, 10);
#else
								(uint64)wcstoll(sData.cwstr(), &pEnd, 10);
#endif

							return CommonLib::CVariant(nVal);
						}
						break;
					case dtBlob:
						{
							CommonLib::CBlob *pBlob = new CommonLib::CBlob((byte*)sData.cstr(), sData.length(), false);
							return CommonLib::CVariant(CommonLib::IRefObjectPtr((CommonLib::IRefCnt*)pBlob));
						}
						break;
				}
			}
		}
	}
}