#ifndef _LIB_COMMON_BASE_VARIANT_H_
#define _LIB_COMMON_BASE_VARIANT_H_
#include "general.h"
#include "Variant.h"

namespace CommonLib
{
	
	template <class Type, int DataType>
	class BaseVariant : public IVariant
	{
	public:
		typedef Type TVarType;
		typedef BaseVariant<Type, DataType> TSelfType;
		BaseVariant() : m_bIsEmpty(true)
		{}
		BaseVariant(const Type& value) : m_Value(value), m_bIsEmpty(true)
		{}
		virtual IVariant* clone() const 
		{
			return new TSelfType(m_Value);
		}

		virtual bool isEmpty(bool) const
		{
			return m_bIsEmpty;
		}
		virtual bool setEmpty()
		{
			m_bIsEmpty = true;
			return true;
		}
		virtual uint16 getType() const
		{
			return DataType;
		}
		virtual bool set( const Type& value)
		{ 
			m_Value = value;
			m_bIsEmpty = false;
			return true;
		}
		virtual bool get(Type& value)
		{ 
			value = m_Value;
			return true;
		}

		virtual bool LE(const IVariant* pVariant) const
		{
			const BaseVariant* pBaseVariant = (BaseVariant*)pVariant;
			if(!pBaseVariant)
				return false;

			return m_Value < pBaseVariant->m_Value;
		}
		virtual bool EQ(const IVariant* pVariant) const
		{
			const BaseVariant* pBaseVariant = (BaseVariant*)pVariant;
			if(!pBaseVariant)
				return false;
			return m_Value == pBaseVariant->m_Value;
		}
		virtual void load(CommonLib::IReadStream *pStream)
		{
			pStream->read(m_Value);
		}
		virtual void save(CommonLib::IWriteStream *pStream)
		{
			pStream->write(m_Value);
		}
		bool copy(const IVariant *pVariant)
		{
			assert(pVariant->getType() == getType());
			const BaseVariant* pBaseVariant = (BaseVariant*)pVariant;
			if(!pBaseVariant)
				return false;
			m_bIsEmpty = pBaseVariant->m_bIsEmpty;
			m_Value = pBaseVariant->m_Value;
			return true;

		}
	protected:
		TVarType m_Value;
		bool m_bIsEmpty;
	};

	typedef BaseVariant<char, dtInteger8> TVarINT8;
	typedef BaseVariant<byte, dtUInteger8> TVarUINT8;
	typedef BaseVariant<int16, dtInteger16> TVarINT16;
	typedef BaseVariant<uint16, dtUInteger16> TVarUINT16;
	typedef BaseVariant<int32, dtInteger32> TVarINT32;
	typedef BaseVariant<uint32, dtUInteger32> TVarUINT32;
	typedef BaseVariant<int64, dtInteger64> TVarINT64;
	typedef BaseVariant<uint64, dtUInteger64> TFVarUINT64;
	typedef BaseVariant<float, dtFloat> TVarFloat;
	typedef BaseVariant<double, dtDouble> TVarDouble;
	typedef BaseVariant<str_t, dtString> TVarString;
}
#endif