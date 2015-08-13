#ifndef GIS_ENGINE_GEO_DATABASE_OIDSET_H
#define GIS_ENGINE_GEO_DATABASE_OIDSET_H

#include "GeoDatabase.h"
 

namespace GisEngine
{
	namespace GeoDatabase
	{

		class  COIDSet : public IOIDSet
		{
		public:
			COIDSet();
			virtual ~COIDSet();

		private:
			COIDSet(const COIDSet&);
			COIDSet& operator=(const COIDSet&);

		public:
			// IOIDSet
			virtual int  GetCount() const;
			virtual bool Find(int id) const;
			virtual void Reset();
			virtual bool Next(int* id);
			virtual void Add(int id);
			virtual void AddList(const int* id, int count);
			virtual void Remove(int id);
			virtual void RemoveList(const int* id, int count);
			virtual void Clear();
		private:
			typedef std::set<int> TSetIDs;
			TSetIDs                 m_setIds;
			TSetIDs::iterator       m_it;
		};
	}
}

#endif