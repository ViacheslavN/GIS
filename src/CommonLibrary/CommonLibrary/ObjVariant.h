#ifndef _LIB_COMMON_OBJ_VARIANT_H_
#define _LIB_COMMON_OBJ_VARIANT_H_
#include "general.h"
#include "IVariant.h"
#include "IGeoShape.h"
namespace CommonLib
{

	template <class ObjType, int DataType>
	class ObjVariant : public IVariant
	{
	public:
		typedef ObjType TVarType;
		typedef ObjVariant<ObjType, DataType> TSelfType;
		ObjVariant() : m_bIsEmpty(true)
		{}
		ObjVariant(const ObjType& value) : m_Value(value), m_bIsEmpty(true)
		{}
		virtual IVariant* clone() const 
		{
			return new TSelfType(m_Value); //TO DO USE alloc
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
		virtual bool set( const ObjType& value)
		{ 
			m_Value = value;
			m_bIsEmpty = false;
			return true;
		}
		virtual bool get(ObjType& value)
		{ 
			value = m_Value;
			return true;
		}

		virtual bool LE(const IVariant* pVariant) const
		{
			const ObjVariant* pObjVariant = (ObjVariant*)pVariant;
			if(!pObjVariant)
				return false;

			return m_Value < pObjVariant->m_Value;
		}
		virtual bool EQ(const IVariant* pVariant) const
		{
			const ObjVariant* pObjVariant = (ObjVariant*)pVariant;
			if(!pObjVariant)
				return false;
			return m_Value == pObjVariant->m_Value;
		}
		virtual void load(CommonLib::IReadStream *pStream)
		{
		}
		virtual void save(CommonLib::IWriteStream *pStream)
		{
		}
		bool copy(const IVariant *pVariant)
		{
			assert(pVariant->getType() == getType());
			const ObjVariant* pObjVariant = (ObjVariant*)pVariant;
			if(!pObjVariant)
				return false;
			m_bIsEmpty = pObjVariant->m_bIsEmpty;
			m_Value = pObjVariant->m_Value;
			return true;

		}


	protected:
		TVarType m_Value;
		bool m_bIsEmpty;
	};
	
	typedef ObjVariant<IGeoShapePtr, dtGeometry> TVarGeometry;
}
#endif