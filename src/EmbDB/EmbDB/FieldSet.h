#ifndef _EMBEDDED_DATABASE_FIELD_SET_H_
#define _EMBEDDED_DATABASE_FIELD_SET_H_


#include "embDB.h"
#include <map>
namespace embDB
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
		virtual int Find(const CommonLib::CString& field) const;
		virtual void Reset();
		virtual bool Next(CommonLib::CString* field);
		virtual void Add(const CommonLib::CString& field);
		virtual void Remove(const CommonLib::CString& field);
		virtual	const CommonLib::CString& Get(int nIndex) const;
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



#endif