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
			m_vecVariants[i]->~IVariant();
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
	CommonLib::IVariant * CompositeIndexKey::getValue(uint32 nNum)
	{
		assert(nNum < m_vecVariants.size());
		return m_vecVariants[nNum];
	}
	const CommonLib::IVariant * CompositeIndexKey::getValue(uint32 nNum) const
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
			CommonLib::IVariant *pVariant = createVariant(vecScheme[i]);
			if(!pVariant)
				return false;
			pVariant->load(&stream);
			m_vecVariants.push_back(pVariant);
		}
		return true;
	}
	bool CompositeIndexKey::setValue(uint32 nNum, const CommonLib::IVariant* pValue)
	{
		assert(nNum < m_vecVariants.size());
		return m_vecVariants[nNum]->copy(pValue);
	}
	bool CompositeIndexKey::addValue(const CommonLib::IVariant* pValue)
	{
		CommonLib::IVariant* pVariant =  createVariant(pValue->getType());
		if(!pValue)
			return false;
		if(!pVariant->copy(pValue))
			return false;
		m_vecVariants.push_back(pVariant);
		return true;
	}
	CommonLib::IVariant* CompositeIndexKey::createVariant(uint16 nType)
	{
		switch(nType)
		{
			case CommonLib::dtUInteger8:
				return new (m_pAlloc->alloc(sizeof(CommonLib::TVarUINT8))) CommonLib::TVarUINT8();
				break;
			case CommonLib::dtInteger8:
				return new (m_pAlloc->alloc(sizeof(CommonLib::TVarINT8))) CommonLib::TVarINT8();
				break;
			case CommonLib::dtUInteger16:
				return new (m_pAlloc->alloc(sizeof(CommonLib::TVarUINT16))) CommonLib::TVarUINT16();
				break;
			case CommonLib::dtInteger16:
				return new (m_pAlloc->alloc(sizeof(CommonLib::TVarINT16))) CommonLib::TVarINT16();
				break;
			case CommonLib::dtUInteger32:
				return new (m_pAlloc->alloc(sizeof(CommonLib::TVarUINT32))) CommonLib::TVarUINT32();
				break;
			case CommonLib::dtInteger32:
				return new (m_pAlloc->alloc(sizeof(CommonLib::TVarINT32))) CommonLib::TVarINT32();
				break;
			case CommonLib::dtUInteger64:
				return new (m_pAlloc->alloc(sizeof(CommonLib::TFVarUINT64))) CommonLib::TFVarUINT64();
				break;
			case CommonLib::dtInteger64:
				return new (m_pAlloc->alloc(sizeof(CommonLib::TVarINT64))) CommonLib::TVarINT64();
				break;
			case CommonLib::dtFloat:
				return new (m_pAlloc->alloc(sizeof(CommonLib::TVarFloat))) CommonLib::TVarFloat();
				break;
			case CommonLib::dtDouble:
				return new (m_pAlloc->alloc(sizeof(CommonLib::TVarDouble))) CommonLib::TVarDouble();
				break;
		}

		return NULL;

	}
}