#include "stdafx.h"
#include "CompositeIndex.h"
#include "VariantField.h"
namespace embDB
{
	CompositeIndexKey::CompositeIndexKey(CommonLib::alloc_t *pAlloc) : m_pAlloc(pAlloc)
	{

	}
	CompositeIndexKey::~CompositeIndexKey()
	{
		for (size_t i = 0, sz = m_vecVariants.size(); i < sz; ++i)
		{
			m_vecVariants[i]->~IFieldVariant();
			m_pAlloc->free(m_vecVariants[i]);
		}

		m_vecVariants.clear();
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

	IFieldVariant* CompositeIndexKey::createVariant(uint16 nType)
	{
		switch(nType)
		{
			case ftUInteger8:
				return new (m_pAlloc->alloc(sizeof(TFieldUINT8))) TFieldUINT8();
				break;
			case ftInteger8:
				return new (m_pAlloc->alloc(sizeof(TFieldUINT8))) TFieldINT8();
				break;
		}

		return NULL;

	}
}