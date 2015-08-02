#ifndef GIS_ENGINE_GEO_DATABASE_FIELDSET_H
#define GIS_ENGINE_GEO_DATABASE_FIELDSET_H

#include "GeoDatabase.h"


namespace GisEngine
{
	namespace GeoDatabase
	{

		class CFieldSet : public IFieldSet
		{

		public:
			CFieldSet();
			virtual ~CFieldSet();
		private:
			CFieldSet(const CFieldSet&);
			CFieldSet& operator=(const CFieldSet&);

		public:
			// IFieldSet
			virtual int  GetCount() const;
			virtual bool Find(const CommonLib::str_t& field) const;
			virtual void Reset();
			virtual bool Next(CommonLib::str_t* field);
			virtual void Add(const CommonLib::str_t& field);
			virtual void Remove(const CommonLib::str_t& field);
			virtual void Clear();

		private:
			struct GisStringCILess
			{
				bool operator ()(const CommonLib::str_t& str1, const CommonLib::str_t& str2) const
				{
					return str1.compare(str2, false) < 0;
				}
			};
			typedef std::map<CommonLib::str_t, int, GisStringCILess> TMapIndexes;
			typedef std::vector<CommonLib::str_t> TVecFields;
			TMapIndexes m_mapIndexes;
			TVecFields  m_vecFields;
			TVecFields::iterator m_it;

		};

	}
}

#endif