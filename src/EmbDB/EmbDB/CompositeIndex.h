#ifndef _EMBEDDED_DATABASE_COMPOSITE_INDEX_H_
#define _EMBEDDED_DATABASE_COMPOSITE_INDEX_H_

#include "IField.h"
#include "CommonLibrary/alloc_t.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "BPVector.h"

namespace embDB
{



	class CompositeIndexKey
	{
	public:
		CompositeIndexKey(CommonLib::alloc_t *pAlloc = nullptr);
		~CompositeIndexKey();

		CompositeIndexKey(const CompositeIndexKey& key);
		CompositeIndexKey& operator=(const CompositeIndexKey& key);
		
		bool LE(const CompositeIndexKey& key) const;
		bool EQ(const CompositeIndexKey& key) const;

		bool operator < (const CompositeIndexKey& key) const;
		bool operator == (const CompositeIndexKey& key) const;

		uint32 getSize() const;
 
		IFieldVariant * getValue(uint32 nNum);
		const IFieldVariant * getValue(uint32 nNum) const;
		bool setValue(uint32 nNum, const IFieldVariant* pValue);
		bool addValue(const IFieldVariant* pValue);

		void write(CommonLib::FxMemoryWriteStream& stream);
		bool load(const std::vector<uint16>& vecScheme, CommonLib::FxMemoryReadStream& stream);
	private:
		void clear();
		IFieldVariant* createVariant(uint16 nType);
	private:
		TBPVector<IFieldVariant*> m_vecVariants;
		CommonLib::alloc_t *m_pAlloc;

	};



}

#endif