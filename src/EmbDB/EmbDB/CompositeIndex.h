#ifndef _EMBEDDED_DATABASE_COMPOSITE_INDEX_H_
#define _EMBEDDED_DATABASE_COMPOSITE_INDEX_H_

#include "IField.h"
#include "CommonLibrary/FixedMemoryStream.h"


namespace embDB
{



	class CompositeIndexKey
	{
	public:
		CompositeIndexKey(uint32 nNum = 0);
		~CompositeIndexKey();
		bool LE(const CompositeIndexKey& key) const;
		bool EQ(const CompositeIndexKey& key) const;

		uint32 getRowSize();
		uint32 getFields();
 
		IFieldVariant * getValue(uint32 nNum);
		bool setValue(uint32 nNum, const IFieldVariant* pValue);
		bool addValue(const IFieldVariant* pValue);

		void write(CommonLib::FxMemoryWriteStream& stream);
		void load(eDataTypes* scheme , uint32 nNum, CommonLib::FxMemoryReadStream& stream);
	private:
		std::vector<IFieldVariant*> m_vecVariants;

	};



}

#endif