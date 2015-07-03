#include "stdafx.h"
#include "CompositeIndexKey.h"
#include "VariantField.h"
namespace embDB
{
	CompositeIndexKey::CompositeIndexKey(CommonLib::alloc_t *pAlloc) : m_pAlloc(pAlloc),
		m_vecVariants(pAlloc)
	{}
	CompositeIndexKey::~CompositeIndexKey()
	{
		clear();
	}
	void CompositeIndexKey::clear()
	{

		for (size_t i = 0, sz = m_vecVariants.size(); i < sz; ++i)
		{
			m_vecVariants[i]->~IFieldVariant();
			m_pAlloc->free(m_vecVariants[i]);
		}

		m_vecVariants.clear();
		m_pAlloc = NULL;
	}
	CompositeIndexKey::CompositeIndexKey(const CompositeIndexKey& key) : 
		m_pAlloc(key.m_pAlloc), m_vecVariants(key.m_pAlloc)
	{
		if(key.getSize() == 0)
			return;

		m_vecVariants.reserve(key.getSize());
		for (size_t i = 0, sz = key.getSize(); i < sz; ++i)
		{
			addValue(key.getValue(i));
		}

	}
	CompositeIndexKey& CompositeIndexKey::operator=(const CompositeIndexKey& key)
	{
		if(key.getSize() == 0)
		{
			clear();
			m_pAlloc = key.m_pAlloc;
			return *this;
		}
		if(m_pAlloc == key.m_pAlloc)
		{
			if(key.getSize() != getSize())
				m_vecVariants.resize(key.getSize());
			for (size_t i = 0, sz = key.getSize(); i < sz; ++i)
			{
				setValue(i, key.getValue(i));
			}
			return *this;
		}

		clear();
		m_pAlloc = key.m_pAlloc;
		m_vecVariants.setAlloc(m_pAlloc);
		m_vecVariants.reserve(key.getSize());
		for (size_t i = 0, sz = key.getSize(); i < sz; ++i)
		{
			addValue(key.getValue(i));
		}
		return *this;
	}
	bool CompositeIndexKey::LE(const CompositeIndexKey& key) const
	{
		uint32 nNum = min(getSize(), key.getSize());
		for (uint32 i = 0; i < nNum; ++i)
		{
			if(!m_vecVariants[i]->EQ(key.getValue(i)))
				return m_vecVariants[i]->LE(key.getValue(i));
		}
		return false;
	}
	bool CompositeIndexKey::EQ(const CompositeIndexKey& key) const
	{
		if(getSize() != key.getSize())
			return false;
 
		for (uint32 i = 0; i < getSize(); ++i)
		{
			if(!m_vecVariants[i]->EQ(key.getValue(i)))
				return false;
		}
		return true;
	}

	bool CompositeIndexKey::operator < (const CompositeIndexKey& key) const
	{
		return LE(key);
	}
	bool CompositeIndexKey::operator == (const CompositeIndexKey& key) const
	{
		return EQ(key);
	}
	bool CompositeIndexKey::operator != (const CompositeIndexKey& key) const
	{
		return !EQ(key);
	}
	uint32 CompositeIndexKey::getSize() const
	{
		return m_vecVariants.size();
	}
	IFieldVariant * CompositeIndexKey::getValue(uint32 nNum)
	{
		assert(nNum < m_vecVariants.size());
		return m_vecVariants[nNum];
	}
	const IFieldVariant * CompositeIndexKey::getValue(uint32 nNum) const
	{
		assert(nNum < m_vecVariants.size());
		return m_vecVariants[nNum];
	}

	void CompositeIndexKey::write(CommonLib::FxMemoryWriteStream& stream)
	{
		for (size_t i = 0, sz = m_vecVariants.size(); i < sz; ++i)
		{
			m_vecVariants[i]->save(&stream);
		}
	}
	bool CompositeIndexKey::load(const std::vector<uint16>& vecScheme,  CommonLib::FxMemoryReadStream& stream)
	{
		for (size_t i = 0, sz = vecScheme.size(); i < sz; ++i)
		{
			IFieldVariant *pVariant = createVariant(vecScheme[i]);
			if(!pVariant)
				return false;
			pVariant->load(&stream);
			m_vecVariants.push_back(pVariant);
		}
		return true;
	}
	bool CompositeIndexKey::setValue(uint32 nNum, const IFieldVariant* pValue)
	{
		assert(nNum < m_vecVariants.size());
		return m_vecVariants[nNum]->copy(pValue);
	}
	bool CompositeIndexKey::addValue(const IFieldVariant* pValue)
	{
		IFieldVariant* pVariant =  createVariant(pValue->getType());
		if(!pValue)
			return false;
		if(!pVariant->copy(pValue))
			return false;
		m_vecVariants.push_back(pVariant);
		return true;
	}
	IFieldVariant* CompositeIndexKey::createVariant(uint16 nType)
	{
		switch(nType)
		{
			case ftUInteger8:
				return new (m_pAlloc->alloc(sizeof(TFieldUINT8))) TFieldUINT8();
				break;
			case ftInteger8:
				return new (m_pAlloc->alloc(sizeof(TFieldINT8))) TFieldINT8();
				break;
			case ftUInteger16:
				return new (m_pAlloc->alloc(sizeof(TFieldUINT16))) TFieldUINT16();
				break;
			case ftInteger16:
				return new (m_pAlloc->alloc(sizeof(TFieldINT16))) TFieldINT16();
				break;
			case ftUInteger32:
				return new (m_pAlloc->alloc(sizeof(TFieldUINT32))) TFieldUINT32();
				break;
			case ftInteger32:
				return new (m_pAlloc->alloc(sizeof(TFieldINT32))) TFieldINT32();
				break;
			case ftUInteger64:
				return new (m_pAlloc->alloc(sizeof(TFieldUINT64))) TFieldUINT64();
				break;
			case ftInteger64:
				return new (m_pAlloc->alloc(sizeof(TFieldINT64))) TFieldINT64();
				break;
			case ftFloat:
				return new (m_pAlloc->alloc(sizeof(TFieldFloat))) TFieldFloat();
				break;
			case ftDouble:
				return new (m_pAlloc->alloc(sizeof(TFieldDouble))) TFieldDouble();
				break;
		}

		return NULL;

	}
}