#include "stdafx.h"
#include "embDBUtils.h"
#include "../../EmbDB/EmbDB/embDBInternal.h"
#include "Field.h"
#include "Fields.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace embDBUtils
		{

			IFieldPtr EmbDBField2Field(embDB::IField *pEmbDBField)
			{
				if(!pEmbDBField)
					return IFieldPtr();

				CField *pField = new CField();

				pField->SetName(pEmbDBField->getName());
				pField->SetAliasName(pEmbDBField->getAlias());
				pField->SetLength(pEmbDBField->GetLength());
				pField->SetIsNullable(!pEmbDBField->GetIsNotEmpty());
				pField->SetType(EmbDBType2FieldType(pEmbDBField->getType()));

				return IFieldPtr(pField);

			}
			IFieldsPtr EmbDBFields2Fields(embDB::IFields *pEmbDBFields)
			{
				if(!pEmbDBFields)
					return IFieldsPtr();

				
				CFields* pFields = new CFields();
				for (size_t i = 0 , sz = pEmbDBFields->GetFieldCount(); i < sz; ++i)
				{
					IFieldPtr pField =  EmbDBField2Field(pEmbDBFields->GetField(i).get());
					pFields->AddField(pField.get());
				}
				
				return IFieldsPtr(pFields);
			}
			embDB::eSpatialCoordinatesUnits GetGeometryUnits(GisEngine::GisCommon::Units units)
			{
				switch(units)
				{
				case GisEngine::GisCommon::UnitsUnknown:
					return embDB::scuUnknown;
					break;
				case GisEngine::GisCommon::UnitsInches:
					return embDB::scuInches;
					break;
				case GisEngine::GisCommon::UnitsPoints:
					return embDB::scuPoints;
					break;
				case GisEngine::GisCommon::UnitsFeet:
					return embDB::scuFeet;
					break;
				case GisEngine::GisCommon::UnitsYards:
					return embDB::scuYards;
					break;
				case GisEngine::GisCommon::UnitsMiles:
					return embDB::scuMiles;
					break;
				case GisEngine::GisCommon::UnitsNauticalMiles:
					return embDB::scuNauticalMiles;
					break;
				case GisEngine::GisCommon::UnitsMillimeters:
					return embDB::scuMillimeters;
					break;
				case GisEngine::GisCommon::UnitsCentimeters:
					return embDB::scuCentimeters;
					break;
				case GisEngine::GisCommon::UnitsMeters:
					return embDB::scuMeters;
					break;
				case GisEngine::GisCommon::UnitsKilometers:
					return embDB::scuKilometers;
					break;
				case GisEngine::GisCommon::UnitsDecimalDegrees:
					return embDB::scuDecimalDegrees;
					break;
				case GisEngine::GisCommon::UnitsDecimeters:
					return embDB::scuDecimeters;
					break;
				}

				return embDB::scuUnknown;
			}
			eDataTypes EmbDBType2FieldType(embDB::eDataTypes type)
			{
				switch(type)
				{
				case embDB::dtInteger8:
					return dtInteger8;
					break;
				case embDB:: dtInteger16:
					return dtInteger16;
					break;
				case embDB::dtInteger32:
					return dtInteger32;
					break;
				case embDB::dtInteger64:
					return dtInteger64;
					break;
				case embDB::dtUInteger8:
					return dtUInteger8;
					break;
				case embDB::dtUInteger16:
					return dtUInteger16;
					break;
				case embDB::dtUInteger32:
					return dtUInteger32;
					break;
				case embDB::dtUInteger64:
					return dtUInteger64;
					break;
			 
				case embDB::dtDouble:
					return dtDouble;
					break;
				case  embDB::dtBlob:
					return dtBlob;
					break;
				case  embDB::dtGeometry:
					return dtGeometry;
					break;
				case  embDB::dtString:
					return dtString;
					break;
			 
				}
				return dtUnknown;
			}
			embDB::eDataTypes FieldType2EmbDBType(eDataTypes type, CommonLib::CString* pStringType)
			{
				switch(type)
				{
				case dtInteger8:
					{
						if(pStringType)
							*pStringType = L"Integer8";
						return embDB::dtInteger8;
							 
					}
					break;
				case dtInteger16:
					{
						if(pStringType)
							*pStringType = L"Integer16";
						return embDB::dtInteger16;
					}
					break;
				case dtInteger32:
					{
						if(pStringType)
							*pStringType = L"Integer32";
						return embDB::dtInteger32;
					}
					break;
				case dtInteger64:
					{
						if(pStringType)
							*pStringType = L"Integer64";
						return embDB::dtInteger64;
					}
					break;
				case dtUInteger8:
					{
						if(pStringType)
							*pStringType = L"Unsigned Integer8";
						return embDB::dtUInteger8;
					}
					break;
				case dtUInteger16:
					{
						if(pStringType)
							*pStringType = L"Unsigned Integer16";
						return embDB::dtUInteger16;
					}
					break;
				case dtUInteger32:
					{
						if(pStringType)
							*pStringType = L"Unsigned Integer32";
						return embDB::dtUInteger32;
					}
					break;
				case dtUInteger64:
					{
						if(pStringType)
							*pStringType = L"Unsigned Integer64";
						return embDB::dtUInteger64;
					}
					break;
				case dtOid32:
					{
						if(pStringType)
							*pStringType = L"Integer32";
						return embDB::dtInteger32;
					}
					break;
				case dtOid64:
					{
						if(pStringType)
							*pStringType = L"Unsigned Integer64";
						return embDB::dtUInteger64;
					}
					break;
				case dtFloat:
					{
						if(pStringType)
							*pStringType = L"Float";
						return embDB::dtFloat;
					}
					break;
				case dtDouble:
					{
						if(pStringType)
							*pStringType = L"Double";
						return embDB::dtDouble;
					}
					break;
				case  dtBlob:
					{
						if(pStringType)
							*pStringType = L"Blob";
						return embDB::dtBlob;
					}
					break;
				case  dtGeometry:
					{
						if(pStringType)
							*pStringType = L"Geometry";
						return embDB::dtGeometry;
					}
					break;
				case  dtString:
					{
						if(pStringType)
							*pStringType = L"String";
						return embDB::dtString;
					}
					break;
				case  dtAnnotation:
					{
						if(pStringType)
							*pStringType = L"String";
						return embDB::dtString;
					}
					break;
				}

				return embDB::dtUnknown;
			}
		
			bool CreateTable(const wchar_t* pszTableName, IFields* pFields, embDB::IDatabase *pDatabase, CommonLib::CString* pOIDField, CommonLib::CString* pShapeFieldName, 
				CommonLib::CString* pAnno , CommonLib::eShapeType *pSPType , GisGeometry::ISpatialReferencePtr* pSPref)
			{
				if(!pFields || !pDatabase)
					return false;

				
			 


				//TO DO create sql operation

				embDB::ISchemaPtr pSchema = pDatabase->getSchema();
				embDB::ITablePtr  pTable = pSchema->getTableByName(pszTableName);
				if(pTable.get())
					return false;

				pSchema->addTable(pszTableName);
				pTable = pSchema->getTableByName(pszTableName);
				if(!pTable.get())
					return false;

				embDB::IDBTable *pDBTable = dynamic_cast<embDB::IDBTable *>(pTable.get()); 
				if(!pDBTable)
					return false;

				for (size_t i = 0, sz = pFields->GetFieldCount(); i < sz; ++i)
				{
					
					IFieldPtr pField = pFields->GetField(i);
					const CommonLib::CVariant& defVal = pField->GetDefaultValue();
					embDB::eDataTypes dt = FieldType2EmbDBType(pField->GetType(), NULL);

					if(pField->GetType() == dtOid32 ||pField->GetType() == dtOid64 )
					{
						if(pOIDField)
							*pOIDField = pField->GetName();
					}
					else if(pField->GetType() == dtAnnotation)
					{
						if(pAnno)
							*pAnno = pField->GetName();
					}

					if(dt == embDB::dtGeometry)
					{
						IShapeField *pShapeField = dynamic_cast<IShapeField*>(pField.get());
						if(!pShapeField)
							return false;
						IGeometryDefPtr pGeometry = pShapeField->GetGeometryDef();
						GisGeometry::ISpatialReferencePtr pSpatialRef = pGeometry->GetSpatialReference();
						CommonLib::eShapeType shapeType =  pGeometry->GetGeometryType();
						CommonLib::bbox extent = pGeometry->GetBaseExtent();
						embDB::eSpatialCoordinatesUnits units = embDB::scuUnknown;

						if(pSpatialRef.get())
							units = GetGeometryUnits(pSpatialRef->GetUnits());

						if(pShapeField)
							*pShapeFieldName = pField->GetName();
						if(pSPType)
							*pSPType = shapeType;
						if(pSPref)
							*pSPref = pSpatialRef;

						if(!pDBTable->createShapeField(pField->GetName().cwstr(), pField->GetAliasName().cwstr(), shapeType, extent, units))
							return false;
					}
					else
					{
						embDB::SFieldProp fp;

						fp.dataType = dt;
						fp.dateTypeExt = embDB::dteSimple;
						if(!pField->GetIsNullable())
							fp.dateTypeExt |= embDB::dteIsNotEmpty;


						fp.sFieldName = pField->GetName();
						fp.sFieldAlias = pField->GetAliasName();
						fp.devaultValue = pField->GetDefaultValue();
						fp.nLenField = pField->GetLength();
						if(!pDBTable->createField(fp))
							return false;
					}

					

				}
				return true;
			}

			embDB::eTransactionType  TranType2EmbDbTranType(eTransactionType type)
			{
				switch(type)
				{
					case ttUndefined:
						return embDB::eTT_UNDEFINED;
						break;
					case ttSelect:
						return embDB::eTT_SELECT;
						break;
					case ttModify:
						return embDB::eTT_MODIFY;
						break;
					case ttDDL:
						return embDB::eTT_DDL;
						break;
				}

				return embDB::eTT_UNDEFINED;
			}
		}
	}
}