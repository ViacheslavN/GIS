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
			virtual bool Find(const CommonLib::CString& field) const;
			virtual void Reset();
			virtual bool Next(CommonLib::CString* field);
			virtual void Add(const CommonLib::CString& field);
			virtual void Remove(const CommonLib::CString& field);
			virtual void Clear();

		private:
			struct GisStringCILess
			{
				bool operator ()(const CommonLib::CString& str1, const CommonLib::CString& str2) const
				{
					return str1.compare(str2, false) < 0;
				}
			};
			typedef std::map<CommonLib::CString, int, GisStringCILess> TMapIndexes;
			typedef std::vector<CommonLib::CString> TVecFields;
			TMapIndexes m_mapIndexes;
			TVecFields  m_vecFields;
			TVecFields::iterator m_it;

		};

	}
}

#endif