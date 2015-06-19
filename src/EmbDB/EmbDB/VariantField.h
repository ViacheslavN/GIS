#ifndef _EMBEDDED_DATABASE_VARIANT_FIELD_H_
#define _EMBEDDED_DATABASE_VARIANT_FIELD_H_
#include "key.h"
#include "CommonLibrary/general.h"
#include "IField.h"

namespace embDB
{

	
	
	template <class Type, int FieldType>
	class IBaseFieldVariant : public IFieldVariant
	{
	public:
		typedef Type TVarType;
		IBaseFieldVariant() : m_bIsEmpty(true)
		{}
		IBaseFieldVariant(const Type& value) : m_Value(value), m_bIsEmpty(true)
		{}

		virtual bool isEmpty(bool)
		{
			return m_bIsEmpty;
		}
		virtual bool setEmpty()
		{
			m_bIsEmpty = true;
			return true;
		}
		virtual int getType()
		{
			return FieldType;
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

		virtual bool LE(const IFieldVariant* pVariant) const
		{
			const IBaseFieldVariant* pBaseVariant = (IBaseFieldVariant*)pVariant;
			if(!pBaseVariant)
				return false;

			return m_Value < pBaseVariant->m_Value;
		}
		virtual bool EQ(const IFieldVariant* pVariant) const
		{
			const IBaseFieldVariant* pBaseVariant = (IBaseFieldVariant*)pVariant;
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

		protected:
			TVarType m_Value;
			bool m_bIsEmpty;
	};

	typedef IBaseFieldVariant<char, ftInteger8> TFieldINT8;
	typedef IBaseFieldVariant<byte, ftUInteger8> TFieldUINT8;
	typedef IBaseFieldVariant<int16, ftInteger16> TFieldINT16;
	typedef IBaseFieldVariant<uint16, ftUInteger16> TFieldUINT16;
	typedef IBaseFieldVariant<int32, ftInteger32> TFieldINT32;
	typedef IBaseFieldVariant<uint32, ftUInteger32> TFieldUINT32;
	typedef IBaseFieldVariant<int64, ftInteger64> TFieldINT64;
	typedef IBaseFieldVariant<uint64, ftUInteger64> TFieldUINT64;
	typedef IBaseFieldVariant<float, ftFloat> TFieldFloat;
	typedef IBaseFieldVariant<double, ftDouble> TFieldDouble;
}
#endif