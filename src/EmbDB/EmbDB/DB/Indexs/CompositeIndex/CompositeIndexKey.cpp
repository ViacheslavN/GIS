#include "stdafx.h"
#include "CompositeIndexKey.h"
//#include "VariantField.h"
namespace embDB
{
	CompositeIndexKey::CompositeIndexKey(CommonLib::alloc_t *pAlloc) : m_pAlloc(pAlloc)
		//m_vecVariants(pAlloc)
	{}
	CompositeIndexKey::~CompositeIndexKey()
	{
		clear();
	}
	void CompositeIndexKey::clear()
	{
/*
		for (uint32 i = 0, sz = m_vecVariants.size(); i < sz; ++i)
		{
			m_vecVariants[i]->~IVariant();
			m_pAlloc->free(m_vecVariants[i]);
		}
		*/
		m_vecVariants.clear();
		m_pAlloc = NULL;
	}
	CompositeIndexKey::CompositeIndexKey(const CompositeIndexKey& key) : 
		m_pAlloc(key.m_pAlloc)/*, m_vecVariants(key.m_pAlloc)*/
	{
		if(key.getSize() == 0)
			return;

		m_vecVariants.reserve(key.getSize());
		for (uint32 i = 0, sz = key.getSize(); i < sz; ++i)
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
			for (uint32 i = 0, sz = key.getSize(); i < sz; ++i)
			{
				setValue(i, key.getValue(i));
			}
			return *this;
		}

		clear();
		m_pAlloc = key.m_pAlloc;
		//m_vecVariants.setAlloc(m_pAlloc);
		m_vecVariants.reserve(key.getSize());
		for (uint32 i = 0, sz = key.getSize(); i < sz; ++i)
		{
			addValue(key.getValue(i));
		}
		return *this;
	}
	bool CompositeIndexKey::LE(const CompositeIndexKey& key) const
	{
		/*uint32 nNum = min(getSize(), key.getSize());
		for (uint32 i = 0; i < nNum; ++i)
		{
			if(!m_vecVariants[i]->EQ(key.getValue(i)))
				return m_vecVariants[i]->LE(key.getValue(i));
		}*/
		return false;
	}
	bool CompositeIndexKey::EQ(const CompositeIndexKey& key) const
	{
	/*	if(getSize() != key.getSize())
			return false;
 
		for (uint32 i = 0; i < getSize(); ++i)
		{
			if(!m_vecVariants[i]->EQ(key.getValue(i)))
				return false;
		}*/
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
	CommonLib::CVariant* CompositeIndexKey::getValue(uint32 nNum)
	{
		assert(nNum < m_vecVariants.size());
		return &m_vecVariants[nNum];
	}
	const CommonLib::CVariant* CompositeIndexKey::getValue(uint32 nNum) const
	{
		assert(nNum < m_vecVariants.size());
		return &m_vecVariants[nNum];
	}

	void CompositeIndexKey::write(CommonLib::FxMemoryWriteStream& stream)
	{
		for (uint32 i = 0, sz = m_vecVariants.size(); i < sz; ++i)
		{
			//m_vecVariants[i]->save(&stream);
		}
	}
	bool CompositeIndexKey::load(const std::vector<uint16>& vecScheme,  CommonLib::FxMemoryReadStream& stream)
	{
		/*for (uint32 i = 0, sz = vecScheme.size(); i < sz; ++i)
		{
			IVariant *pVariant = createVariant(vecScheme[i]);
			if(!pVariant)
				return false;
			pVariant->load(&stream);
			m_vecVariants.push_back(pVariant);
		}*/
		return true;
	}
	bool CompositeIndexKey::setValue(uint32 nNum, const CommonLib::CVariant* pValue)
	{
		/*assert(nNum < m_vecVariants.size());
		return m_vecVariants[nNum]->copy(pValue);*/
		return true;
	}
	bool CompositeIndexKey::addValue(const CommonLib::CVariant* pValue)
	{
		/*IVariant* pVariant =  createVariant(pValue->getType());
		if(!pValue)
			return false;
		if(!pVariant->copy(pValue))
			return false;
		m_vecVariants.push_back(pVariant);*/
		return true;
	}
	/*CommonLib::CVariant* CompositeIndexKey::createVariant(uint16 nType)
	{
		switch(nType)
		{
			case dtUInteger8:
				return new (m_pAlloc->alloc(sizeof(TVarUINT8))) TVarUINT8();
				break;
			case dtInteger8:
				return new (m_pAlloc->alloc(sizeof(TVarINT8))) TVarINT8();
				break;
			case dtUInteger16:
				return new (m_pAlloc->alloc(sizeof(TVarUINT16))) TVarUINT16();
				break;
			case dtInteger16:
				return new (m_pAlloc->alloc(sizeof(TVarINT16))) TVarINT16();
				break;
			case dtUInteger32:
				return new (m_pAlloc->alloc(sizeof(TVarUINT32))) TVarUINT32();
				break;
			case dtInteger32:
				return new (m_pAlloc->alloc(sizeof(TVarINT32))) TVarINT32();
				break;
			case dtUInteger64:
				return new (m_pAlloc->alloc(sizeof(TFVarUINT64))) TFVarUINT64();
				break;
			case dtInteger64:
				return new (m_pAlloc->alloc(sizeof(TVarINT64))) TVarINT64();
				break;
			case dtFloat:
				return new (m_pAlloc->alloc(sizeof(TVarFloat))) TVarFloat();
				break;
			case dtDouble:
				return new (m_pAlloc->alloc(sizeof(TVarDouble))) TVarDouble();
				break;
		}

		return NULL;

	}*/
}