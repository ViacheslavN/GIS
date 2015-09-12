#ifndef _EMBEDDED_DATABASE_I_DB_SHEMA_H_
#define _EMBEDDED_DATABASE_I_DB_SHEMA_H_
#include "CommonLibrary/String.h"
#include "IShema.h"

namespace embDB
{
	enum eLinkType
	{
		ltOneToOne,
		ltOneToMany,
		ltManyToMany

	};
	class ILink
	{
		ILink(){}
		virtual ~ILink(){}
		virtual eLinkType getType() const = 0;
		virtual ITable* getTableFrom() const = 0;
		virtual ITable* getTableTo() const = 0;
		virtual IField* getFieldFrom() const = 0;
		virtual IField* getFieldTo() const = 0;
	};
	class IDBShema : public IShema
	{
	public:
		IDBShema(){}
		virtual ~IDBShema(){}
		
		virtual ILink* AddLink(ITable* pTableFrom, IField* pFieldFrom, ITable* pTableTo, IField* pFieldTo,  eLinkType nLinkType) = 0;
		virtual ILink* AddLink(const CommonLib::CString& sTableFrom, const CommonLib::CString& sFieldFrom, const CommonLib::CString& sTableTo, const CommonLib::CString& sFieldTo,  eLinkType nLinkType) = 0;
		virtual bool deleteLink(ILink *pLink) = 0;
		virtual size_t getLinkCnt() const = 0;
		virtual ILink* getLink(size_t nIndex) const = 0;
	};
}

#endif