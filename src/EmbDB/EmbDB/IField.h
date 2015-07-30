#ifndef _EMBEDDED_DATABASE_I_FIELD_H_
#define _EMBEDDED_DATABASE_I_FIELD_H_
#include "CommonLibrary/str_t.h"
#include "CommonLibrary/SpatialKey.h"
#include "CommonLibrary/IRefCnt.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/Variant.h"
namespace embDB
{


	enum eDataTypesExt
	{
		dteSimple = 0,
		dteIsNotEmpty = 1,
		dteIsUNIQUE = 2,
		dteIsCounter = 4,
		dteIsLink = 8
	};
	enum indexTypes
	{
		itUnknown   = 0x00,
		itUnique,
		itMultiRegular,
		itSpatial,
		itFreeText,
		itRouting,
		itOID
	};


	enum eSpatialCoordinatesType
	{
		sctNotDef =0,
		sctDegrees,
		sctMillimeters,
		sctMeters,
		sctKilometers,
		sctPixels,
		sctUnits
	};

	enum eShapeType
	{
		stNotDef =0,
		stPoint,
		stLine,
		stPolyLine,
		stRect,
		stPolygon,
		stMuitiPolygon
	};

	enum ePointType
	{
		ptNotDef = 0,
		ptUINT16,
		ptUINT32,
		ptUINT64
	};


	enum eGeoSpatialCoordinatesFormat
	{
		gsfNotDef = 0,
		gsfDDDDDDD,
		gsfDDMMSS,
		gsfDDMMMM
	};
	//http://gis-lab.info/qa/dms2dd.html#sel=

	class IField
	{
		public:
			IField(){}
			virtual ~IField(){}
			virtual CommonLib::eDataTypes getType() const = 0;
			virtual const CommonLib::str_t& getName() const = 0;

	};

	/*class IVariant
	{
	public:
		IVariant(){}
		virtual ~IVariant(){}

		virtual bool isEmpty(bool) const { return true;}
		virtual bool setEmpty(){ return false;}
		virtual uint16  getType() const { return CommonLib::dtUnknown;}

		virtual bool LE(const IVariant* pKey) const { return false;}
		virtual bool EQ(const IVariant* pKey) const { return false;}
		virtual bool copy(const IVariant *pVariant) { return false;}

		virtual bool set(bool){ return false;}
		virtual bool set(const byte*, size_t){ return false;}
		virtual bool set(const int16& value){ return false;}
		virtual bool set(const uint16& value){ return false;}
		virtual bool set(const int32& value){ return false;}
		virtual bool set(const uint32& value){ return false;}
		virtual bool set(const int64& value){ return false;}
		virtual bool set(const uint64& value){ return false;}
		virtual bool set(const float& value) { return false;}
		virtual bool set(const double& value){ return false;}
		virtual bool set(const TPoint2D16&	value){ return false;}
		virtual bool set(const TPoint2D32&  value){ return false;}
		virtual bool set(const TPoint2D64&  value){ return false;}
		virtual bool set(const TPoint2Du16&  value){ return false;}
		virtual bool set(const TPoint2Du32&  value){ return false;}
		virtual bool set(const TPoint2Du64&  value){ return false;}
		virtual bool set(const TRect2D16&  value){ return false;}
		virtual bool set(const TRect2D32&  value){ return false;}
		virtual bool set(const TRect2D64&  value){ return false;}
		virtual bool set(const TRect2Du16&  value){ return false;}
		virtual bool set(const TRect2Du32&  value){ return false;}
		virtual bool set(const TRect2Du64&  value){ return false;}


		virtual bool get(bool&){ return false;}
		virtual bool get(byte*, int&) { return false;}
		virtual bool get(int16&){ return false;}
		virtual bool get(uint16&){ return false;}
		virtual bool get(int32&) { return false;}
		virtual bool get(uint32&) { return false;}
		virtual bool get(int64&){ return false;}
		virtual bool get(uint64&){ return false;}
		virtual bool get(float&) { return false;}
		virtual bool get(double&) { return false;}
		virtual bool get(TPoint2D16&){ return false;}
		virtual bool get(TPoint2D32&){ return false;}
		virtual bool get(TPoint2D64&){ return false;}
		virtual bool get(TPoint2Du16&){ return false;}
		virtual bool get(TPoint2Du32&){ return false;}
		virtual bool get(TPoint2Du64&){ return false;}
		virtual bool get(TRect2D16&){ return false;}
		virtual bool get(TRect2D32&){ return false;}
		virtual bool get(TRect2D64&){ return false;}
		virtual bool get(TRect2Du16&){ return false;}
		virtual bool get(TRect2Du32&){ return false;}
		virtual bool get(TRect2Du64&){ return false;}

		virtual void load(CommonLib::IReadStream *pStream){}
		virtual void save(CommonLib::IWriteStream *pStream){}

		template<class TValue>
		bool setVal(const TValue& val)
		{
			return set(val);
		}

		template<class TValue>
		bool getVal(TValue& val)
		{
			return get(val);
		}

	};*/

	
}

#endif