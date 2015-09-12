#ifndef _COMMON_LIB_VARIANT
#define _COMMON_LIB_VARIANT

#include "GeneralTypes.h"
#include "IRefCnt.h"
#include "String.h"
#include "blob.h"
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
#define DECLARE_SIMPLE_TYPE(TT) TT val_##TT;
		union{
			char buffer[MAX_GV_SIZE];
#include "VartTypeList.h"
			std::string *val_str_t;
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
		CVariant(const T& v)
		{
			new(&m_DataBuffer.buffer[0]) T(v);
			m_id = eDataTypes(type2int<T>::typeId);
		}
		bool IsNull() const
		{
			return (m_id == type2int<CNullVariant>::typeId);
		}
		template <typename T>
		bool isType() const
		{
			return (m_id == type2int<T>::typeId);
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
			if (m_id != type2int<T>::typeId)
				assert(false);
			return *(T *)getBuffer();
		}

		template <typename T>
		const T& Get() const
		{
			if (m_id != type2int<T>::typeId)
				assert(false);
			return *(T *)getBuffer();
		}

		template <typename T> 
		T* GetPtr()
		{
			if (m_id != type2int<T>::typeId)
				return NULL;
			return (T *)getBuffer();
		}

		template <typename T> 
		const T* GetPtr() const
		{
			if (m_id != type2int<T>::typeId)
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
	operator CString()const{return val_;};


	virtual void Visit(const CNullVariant&){val_ = CString();}
	virtual void Visit(const bool           & val)
	{ val_ = val ? CString(L"true") : CString(L"false"); }
	virtual void Visit(const byte     & val)
	{ val_.format(L"%d", (int)val); }
	virtual void Visit(const int8     & val)
	{ val_.format(L"%d", (int)val); }
	virtual void Visit(const int16    & val)
	{ val_.format(L"%d", (int)val); }
	virtual void Visit(const uint16    & val)
	{ val_.format(L"%d", (int)val); }
	virtual void Visit(const int32    & val)
	{ val_.format(L"%d", (int)val); }
	virtual void Visit(const uint32   & val)
	{ val_.format(L"%d", (int)val); }
	virtual void Visit(const uint64   & val)
	{ val_.format(L"%I64u", val); }
	virtual void Visit(const int64    & val)
	{ val_.format(L"%UI64d", val); }
	virtual void Visit(const float    & val)
	{ val_.format_c (L"%20.20f", val); }
	virtual void Visit(const double     & val)
	{ val_.format_c (L"%20.20lf", val); }
	virtual void Visit(const CString      & val)
	{ val_ = val;}
	virtual void Visit(const IRefObjectPtr     &)
	{ val_ = CString();}
	virtual void Visit(const CBlob     &)
	{ val_ = CString();}
private:
	CString val_;


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