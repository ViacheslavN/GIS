#ifndef _EMBEDDED_DATABASE_I_FIELD_H_
#define _EMBEDDED_DATABASE_I_FIELD_H_
#include "CommonLibrary/str_t.h"
#include "SpatialKey.h"
#include "IRefCnt.h"
namespace embDB
{
	enum eDataTypes
	{
		ftUnknown   = 0,
		ftNull,
		ftInteger8,
		ftInteger16,
		ftInteger32,
		ftInteger64,
		ftUInteger8,
		ftUInteger16,
		ftUInteger32,
		ftUInteger64,
		ftOid,
		ftFloat,
		ftDouble,
		ftString,
		ftBlob ,
		ftPoint16,
		ftPoint32,
		ftPoint64,
		ftShape16,
		ftShape32,
		ftShape64,
		ftRaster,
		ftRect16,
		ftRect32,
		ftRect64,
		ftSerializedObject
	};
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
			virtual eDataTypes getType() const = 0;
			virtual const CommonLib::str_t& getName() const = 0;

	};

	class IFieldVariant
	{
	public:
		IFieldVariant(){}
		virtual ~IFieldVariant(){}

		virtual bool isEmpty(bool){ return true;}
		virtual bool setEmpty(){ return false;}
		virtual int getType(){ return ftUnknown;}

		virtual bool LE(const IFieldVariant* pKey) const { return false;}
		virtual bool EQ(const IFieldVariant* pKey) const { return false;}

		virtual bool set(bool){ return false;}
		virtual bool set(const byte*, size_t){ return false;}
		virtual bool set(int16 value){ return false;}
		virtual bool set(uint16 value){ return false;}
		virtual bool set(int32 value){ return false;}
		virtual bool set(uint32 value){ return false;}
		virtual bool set(int64 value){ return false;}
		virtual bool set(uint64 value){ return false;}
		virtual bool set(float value) { return false;}
		virtual bool set(double value){ return false;}
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

	};

	
}

#endif