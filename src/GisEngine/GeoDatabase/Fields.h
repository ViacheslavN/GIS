#ifndef GIS_ENGINE_GEO_DATABASE_FIELDS_H
#define GIS_ENGINE_GEO_DATABASE_FIELDS_H

#include "GeoDatabase.h"

namespace GisEngine
{
	namespace GeoDatabase
	{

		class  CFields : public IFields
		{
		
		public:
			CFields();
			virtual ~CFields();
		private:
			CFields(const CFields&);
			CFields& operator=(const CFields&);

		public:
			// IFields
			virtual int       GetFieldCount() const;
			virtual void      SetFieldCount(int count);
			virtual IFieldPtr GetField(int index) const;
			virtual void      SetField(int index, IField* field);
			virtual void      AddField(IField* field);
			virtual void      RemoveField(int index);
			virtual int       FindField(const CommonLib::CString& name) const;
			virtual bool      FieldExists(const CommonLib::CString& name) const;
			virtual void	  Clear();

		private:
			void RebuildFieldIndexMap() const;
			struct GisStringCILess
			{
				bool operator ()(const CommonLib::CString& str1, const CommonLib::CString& str2) const
				{
					return str1.compare(str2, false) < 0;
				}
			};
			typedef std::map<const CommonLib::CString, int, GisStringCILess> TMapFieldIndex;
			typedef std::vector<IFieldPtr> TFields;
			TFields m_vecFields;
			mutable TMapFieldIndex m_mapFieldIndex;
		};
    }
}

#endif