#ifndef _COMMON_LIB_VARIANT
#define _COMMON_LIB_VARIANT

#include "GeneralTypes.h"
#include "IRefCnt.h"
#include "String.h"
#include "blob.h"
#include "SpatialKey.h"
#include "GeoShape.h"
#include "stream.h"
namespace CommonLib
{
	struct CNullVariant
	{
		CNullVariant()
		{}
		~CNullVariant()
		{}
		CNullVariant(const CNullVariant&)
		{}
		bool operator<(const CNullVariant&) const
		{
			return false;
		}
		bool operator<=(const CNullVariant&) const
		{
			return true;
		}
		bool operator>(const CNullVariant&) const
		{
			return false;
		}
		bool operator>=(const CNullVariant&) const
		{
			return true;
		}
		bool operator==(const CNullVariant&) const
		{
			return true;
		}
		bool operator!=(const CNullVariant&) const
		{
			return false;
		}
	};



	class IVisitor
	{
	public:
		IVisitor(){}
		virtual ~IVisitor(){}
		virtual void Visit(const CNullVariant& val) = 0;
		virtual void Visit(const bool           & val) = 0;
		virtual void Visit(const int8          & val) = 0;
		virtual void Visit(const byte            & val) = 0;
		virtual void Visit(const int16          & val) = 0;
		virtual void Visit(const uint16            & val) = 0;
		virtual void Visit(const int32          & val) = 0;
		virtual void Visit(const uint32            & val) = 0;
		virtual void Visit(const int64			 & val) = 0;
		virtual void Visit(const uint64			 & val) = 0;
		virtual void Visit(const float          & val) = 0;
		virtual void Visit(const double         & val) = 0;
		virtual void Visit(const CString      & val) = 0;
		virtual void Visit(const IRefObjectPtr     & val) = 0;
		virtual void Visit(const CBlob     & val) = 0;
		virtual void Visit(const CGeoShape     & val) = 0;
		virtual void Visit(const IGeoShapePtr     & val) = 0;

		virtual void Visit(const TPoint2D16		& val) = 0;
		virtual void Visit(const TPoint2D32     & val) = 0;
		virtual void Visit(const TPoint2D64     & val) = 0;
		virtual void Visit(const TPoint2Du16     & val) = 0;
		virtual void Visit(const TPoint2Du32     & val) = 0;
		virtual void Visit(const TPoint2Du64     & val) = 0;
		virtual void Visit(const TRect2D16      & val) = 0;
		virtual void Visit(const TRect2D32     & val) = 0;
		virtual void Visit(const TRect2D64    & val) = 0;
		virtual void Visit(const TRect2Du16      & val) = 0;
		virtual void Visit(const TRect2Du32     & val) = 0;
		virtual void Visit(const TRect2Du64     & val) = 0;
	};

 

	typedef void (*constructorVariant)(void *,const void *);
	typedef void (*destructorVariant)(void *);
	typedef int  (*compareVariantFunc)(const void *,const void *);
	typedef void (*copyVariantFunc)(void *,const void *);
	typedef void (*assignVariantFunc)(void *,int, const void *);
	typedef void (*acceptVariantFunc)(const void *,IVisitor &);


	template <int> struct maxSize{
		enum { mxSize=4 };
	};
	template <typename T > struct type2int;


	///////////////////// TYPE LIST //////////////////////////////////

#define START_TYPE_LIST() \
	enum {_start_type_list_ = __LINE__ + 1 };

#define _curVal_ (__LINE__-_start_type_list_)

#define FINISH_TYPE_LIST() \
	enum {_type_list_count_ = _curVal_ };



#define DECLARE_TYPE(TT) \
	template<> struct maxSize<_curVal_>{ \
	enum { mxSize=(sizeof(TT) > maxSize<_curVal_-1>::mxSize) ? sizeof(TT) : maxSize<_curVal_-1>::mxSize }; \
	}; \
	template<> struct type2int<TT>{ \
	enum { typeId = _curVal_ }; \
	}; 

#define DECLARE_SIMPLE_TYPE(TT) DECLARE_TYPE(TT)
#define DECLARE_FUNC_TABLES

#include "VartTypeList.h"

#undef DECLARE_FUNC_TABLES
#undef DECLARE_SIMPLE_TYPE
#undef DECLARE_TYPE
#undef START_TYPE_LIST
#undef FINISH_TYPE_LIST
#undef _curVal_

	////////////////////////////////////////////// TypeID ////////////////////////////////

#define START_TYPE_LIST()
#define FINISH_TYPE_LIST()

#define GIS_VAR_TYPE(TT) varType_##TT 

#define DECLARE_TYPE(TT) \
	varType_##TT = type2int<TT>::typeId,

#define DECLARE_SIMPLE_TYPE(TT) DECLARE_TYPE(TT)

	//////////////DataType enum

	enum eDataTypes{
#include "VartTypeList.h"
	};

#undef DECLARE_SIMPLE_TYPE
#undef DECLARE_TYPE


	class CVariant
	{
	protected:
		eDataTypes m_id;

#define DECLARE_TYPE(TT)
#define DECLARE_SIMPLE_TYPE(TT) TT m_val##TT;
		union{
			char buffer[MAX_GV_SIZE];
#include "VartTypeList.h"
			CString *val_str_t;
			CBlob *val_blob;
			CGeoShape *val_shape;
		}m_DataBuffer;
#undef DECLARE_SIMPLE_TYPE
#undef DECLARE_TYPE

	public:

		eDataTypes GetTypeID() const 
		{ 
			return m_id; 
		}
		void *getBuffer(){ return &m_DataBuffer.buffer[0]; }
		const void *getBuffer() const { return &m_DataBuffer.buffer[0]; }

		CVariant()
		{

			m_id = (eDataTypes)(int)type2int<CNullVariant>::typeId;
		}

		CVariant(const CVariant& v)
		{
			m_id = v.GetTypeID();
			var_constructors_[m_id](getBuffer(), v.getBuffer());

		}
		template <typename T>
		void SetType()
		{
			m_id = eDataTypes(type2int<T>::typeId);
		}

		template <typename T>
		CVariant(const T& v)
		{
			new(&m_DataBuffer.buffer[0]) T(v);
			m_id = eDataTypes(type2int<T>::typeId);
		}
		bool IsNull() const
		{
			return (m_id == eDataTypes(type2int<CNullVariant>::typeId));
		}
		template <typename T>
		bool isType() const
		{
			return (m_id == eDataTypes(type2int<T>::typeId));
		}
		CVariant& operator=(const CVariant& variant)
		{
			eDataTypes destId=variant.GetTypeID();
			if(m_id == destId)
				var_copy_[m_id](getBuffer(), variant.getBuffer());
			else
			{
				var_assign_[m_id](getBuffer(), destId, variant.getBuffer());
				m_id = destId;
			}	
			return *this;
		}
		template <typename T>
		CVariant& operator = (const T& value)
		{
			eDataTypes destId = (eDataTypes)type2int<T>::typeId;
			if(m_id == destId)
				var_copy_[m_id](getBuffer(), &value);
			else
			{
				var_assign_[m_id](getBuffer(), destId, &value);
				m_id = destId;
			}	
			return *this;
		}

		template <typename T>
		T& Get()
		{
			if (m_id != (eDataTypes)type2int<T>::typeId)
				assert(false);
			return *(T *)getBuffer();
		}

		template <typename T>
		const T& Get() const
		{
			if (m_id != (eDataTypes)type2int<T>::typeId)
				assert(false);
			return *(T *)getBuffer();
		}

		template <typename T>
		void getVal(T& value) const
		{
			if (m_id != (eDataTypes)type2int<T>::typeId)
				assert(false);
			value = *(T *)getBuffer();
		}
		template <typename T>
		void setVal(const T& value)
		{
			eDataTypes destId = (eDataTypes)type2int<T>::typeId;
			if(m_id == destId)
				var_copy_[m_id](getBuffer(), &value);
			else
			{
				var_assign_[m_id](getBuffer(), destId, &value);
				m_id = destId;
			}	
		 
		}
		
		template <typename T> 
		T* GetPtr()
		{
			if (m_id != (eDataTypes)type2int<T>::typeId)
				return NULL;
			return (T *)getBuffer();
		}

		template <typename T> 
		const T* GetPtr() const
		{
			if (m_id != (eDataTypes)type2int<T>::typeId)
				return NULL;
			return (T *)getBuffer();
		}
		void Accept(IVisitor& visitor) const
		{
			var_accept_[m_id](getBuffer(), visitor);
		}
		int Compare(const CVariant& variant) const
		{
			int destId = variant.GetTypeID();
			int delta = GetTypeID() - destId;
			if(delta == 0)
				return var_compare_[m_id](getBuffer(), variant.getBuffer());
			return delta;
		}


		bool operator == (const CVariant& variant) const
		{
			if(GetTypeID() != variant.GetTypeID())
				return false;
			return (var_compare_[m_id](getBuffer(), variant.getBuffer()) == 0);
		}

		bool operator != (const CVariant& variant) const
		{
			if(GetTypeID() != variant.GetTypeID())
				return true;

			return (var_compare_[m_id](getBuffer(), variant.getBuffer()) != 0);
		}

		bool operator < (const CVariant& variant) const
		{
			int destId = variant.GetTypeID();
			int delta = GetTypeID() - destId;
			if(delta == 0)
				return (var_compare_[m_id](getBuffer(), variant.getBuffer()) < 0);
			return (delta < 0);
		} 
		bool operator <= (const CVariant& variant) const
		{
			int destId = variant.GetTypeID();
			int delta = GetTypeID() - destId;
			if(delta == 0)
				return (var_compare_[m_id](getBuffer(), variant.getBuffer()) <= 0);
			return (delta <= 0);
		} 
		bool operator > (const CVariant& variant) const
		{
			int destId = variant.GetTypeID();
			int delta = GetTypeID() - destId;
			if(delta == 0)
				return (var_compare_[m_id](getBuffer(), variant.getBuffer()) > 0);
			return (delta > 0);
		}
		bool operator >= (const CVariant& variant) const
		{
			int destId = variant.GetTypeID();
			int delta = GetTypeID() - destId;
			if(delta == 0)
				return (var_compare_[m_id](getBuffer(), variant.getBuffer()) >= 0);
			return (delta >= 0);
		}


		


};
class ToStringVisitor : public IVisitor
{
public:
	ToStringVisitor (){}
	operator CString()const{return m_val;};


	virtual void Visit(const CNullVariant&){m_val = CString();}
	virtual void Visit(const bool           & val)
	{ m_val = val ? CString(L"true") : CString(L"false"); }
	virtual void Visit(const byte     & val)
	{ m_val.format(L"%u", (uint32)val); }
	virtual void Visit(const int8     & val)
	{ m_val.format(L"%d", (int)val); }
	virtual void Visit(const int16    & val)
	{ m_val.format(L"%u", (uint32)val); }
	virtual void Visit(const uint16    & val)
	{ m_val.format(L"%d", (int)val); }
	virtual void Visit(const int32    & val)
	{ m_val.format(L"%d", (int)val); }
	virtual void Visit(const uint32   & val)
	{ m_val.format(L"%u", val); }
	virtual void Visit(const uint64   & val)
	{ m_val.format(L"%I64u", val); }
	virtual void Visit(const int64    & val)
	{ m_val.format(L"%UI64d", val); }
	virtual void Visit(const float    & val)
	{ m_val.format_c (L"%20.20f", val); }
	virtual void Visit(const double     & val)
	{ m_val.format_c (L"%20.20lf", val); }
	virtual void Visit(const CString      & val)
	{ m_val = val;}
	virtual void Visit(const IRefObjectPtr     &)
	{ m_val = CString();}
	virtual void Visit(const CBlob     &)
	{ m_val = CString();}
	virtual void Visit(const CGeoShape     &)
	{ m_val = CString();}

	virtual void Visit(const IGeoShapePtr     & val) 
	{
		{ m_val = CString();}
	}

	virtual void Visit(const TPoint2D16	& val)
	{

	}
	virtual void Visit(const TPoint2D32   & val)
	{

	}
	virtual void Visit(const TPoint2D64   & val)
	{

	}
	virtual void Visit(const TPoint2Du16  & val)
	{

	}
	virtual void Visit(const TPoint2Du32  & val)
	{

	}
	virtual void Visit(const TPoint2Du64   & val)
	{

	}
	virtual void Visit(const TRect2D16    & val)
	{

	}
	virtual void Visit(const TRect2D32   & val)
	{

	}
	virtual void Visit(const TRect2D64  & val)
	{

	}
	virtual void Visit(const TRect2Du16    & val)
	{

	}
	virtual void Visit(const TRect2Du32   & val)
	{

	}
	virtual void Visit(const TRect2Du64   & val)
	{

	}
private:
	CString m_val;


};



class ToStreamVisitor : public IVisitor
{
public:
	ToStreamVisitor (IWriteStream *pStream) : m_pStream(pStream)
	{}
 


	virtual void Visit(const CNullVariant&){}
	virtual void Visit(const bool           & val)
	{ m_pStream->write(val);}
	virtual void Visit(const byte     & val)
	{ m_pStream->write(val);}
	virtual void Visit(const int8     & val)
	{ m_pStream->write(val);}
	virtual void Visit(const int16    & val)
	{ m_pStream->write(val);}
	virtual void Visit(const uint16    & val)
	{ m_pStream->write(val);}
	virtual void Visit(const int32    & val)
	{ m_pStream->write(val);}
	virtual void Visit(const uint32   & val)
	{ m_pStream->write(val);}
	virtual void Visit(const uint64   & val)
	{ m_pStream->write(val);}
	virtual void Visit(const int64    & val)
	{ m_pStream->write(val);}
	virtual void Visit(const float    & val)
	{ m_pStream->write(val);}
	virtual void Visit(const double     & val)
	{ m_pStream->write(val);}
	virtual void Visit(const CString      & val)
	{ m_pStream->write(val);}
	virtual void Visit(const IRefObjectPtr     &)
	{ }
	virtual void Visit(const CBlob &val)
	{ m_pStream->write(val.buffer(), val.size());}
	virtual void Visit(const CGeoShape     &val)
	{ val.write(m_pStream);}

	virtual void Visit(const IGeoShapePtr     & val) 
	{
		{ val->write(m_pStream);}
	}

	virtual void Visit(const TPoint2D16	& val)
	{}
	virtual void Visit(const TPoint2D32   & val)
	{}
	virtual void Visit(const TPoint2D64   & val)
	{}
	virtual void Visit(const TPoint2Du16  & val)
	{}
	virtual void Visit(const TPoint2Du32  & val)
	{}
	virtual void Visit(const TPoint2Du64   & val)
	{}
	virtual void Visit(const TRect2D16    & val)
	{}
	virtual void Visit(const TRect2D32   & val)
	{}
	virtual void Visit(const TRect2D64  & val)
	{}
	virtual void Visit(const TRect2Du16    & val)
	{}
	virtual void Visit(const TRect2Du32   & val)
	{}
	virtual void Visit(const TRect2Du64   & val)
	{}
private:
	IWriteStream* m_pStream;
};



template <class TVisitor>
TVisitor apply_visitor (const CVariant& variant, TVisitor &visitor)
{
	variant.Accept (visitor);
	return visitor;
};


#undef START_TYPE_LIST
#undef FINISH_TYPE_LIST

}


#endif