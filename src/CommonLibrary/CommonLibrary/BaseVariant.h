#ifndef _LIB_COMMON_BASE_VARIANT_H_
#define _LIB_COMMON_BASE_VARIANT_H_
#include "general.h"
#include "Variant.h"

namespace CommonLib
{
	
	template <class Type, int DataType>
	class IBaseVariant : public IVariant
	{
	public:
		typedef Type TVarType;
		IBaseVariant() : m_bIsEmpty(true)
		{}
		IBaseVariant(const Type& value) : m_Value(value), m_bIsEmpty(true)
		{}

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
			const IBaseVariant* pBaseVariant = (IBaseVariant*)pVariant;
			if(!pBaseVariant)
				return false;

			return m_Value < pBaseVariant->m_Value;
		}
		virtual bool EQ(const IVariant* pVariant) const
		{
			const IBaseVariant* pBaseVariant = (IBaseVariant*)pVariant;
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
			const IBaseVariant* pBaseVariant = (IBaseVariant*)pVariant;
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

	typedef IBaseVariant<char, dtInteger8> TVarINT8;
	typedef IBaseVariant<byte, dtUInteger8> TVarUINT8;
	typedef IBaseVariant<int16, dtInteger16> TVarINT16;
	typedef IBaseVariant<uint16, dtUInteger16> TVarUINT16;
	typedef IBaseVariant<int32, dtInteger32> TVarINT32;
	typedef IBaseVariant<uint32, dtUInteger32> TVarUINT32;
	typedef IBaseVariant<int64, dtInteger64> TVarINT64;
	typedef IBaseVariant<uint64, dtUInteger64> TFVarUINT64;
	typedef IBaseVariant<float, dtFloat> TVarFloat;
	typedef IBaseVariant<double, dtDouble> TVarDouble;
}
#endif