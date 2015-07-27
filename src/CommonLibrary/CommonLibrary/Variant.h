#ifndef _LIB_COMMON_I_VATIANT_H_
#define _LIB_COMMON_I_VATIANT_H_
#include "SpatialKey.h"
namespace CommonLib
{
	enum eDataTypes
	{
		dtUnknown   = 0,
		dtNull,
		dtInteger8,
		dtInteger16,
		dtInteger32,
		dtInteger64,
		dtUInteger8,
		dtUInteger16,
		dtUInteger32,
		dtUInteger64,
		dtOid,
		dtFloat,
		dtDouble,
		dtString,
		dtBlob ,
		dtPoint16,
		dtPoint32,
		dtPoint64,
		dtShape16,
		dtShape32,
		dtShape64,
		dtRaster,
		dtRect16,
		dtRect32,
		dtRect64,
		dtSerializedObject
	};


	struct STypeSize
	{
		short nLineNo;
		size_t nSize;
	};

	static STypeSize  arrTypeSizes[] = {
		{dtUnknown,			 0},
		{dtNull,			 0},
		{dtInteger8,		 1},
		{dtInteger16,		 2},
		{dtInteger32,		 4},
		{dtInteger64,		 8},
		{dtUInteger8,		 1},
		{dtUInteger16,		 2},
		{dtUInteger32,		 4},
		{dtUInteger64,		 8},
		{dtOid,				 8},
		{dtFloat,			 4},
		{dtDouble,			 8},
		{dtString,			 0},
		{dtBlob,			 8},
		{dtPoint16,			 4},
		{dtPoint32,			 8},
		{dtPoint64,		    16},
		{dtShape16,			 8},
		{dtShape32,		     8},
		{dtShape64,			 8},
		{dtRaster,	 		 8},
		{dtRect16,			 8},
		{dtRect32,			16},
		{dtRect64,			32},
		{dtSerializedObject, 8}

	};

	class IVariant
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

	};
}
#endif