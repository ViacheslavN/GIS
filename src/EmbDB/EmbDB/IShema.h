#ifndef _EMBEDDED_DATABASE_I_SHEMA_H_
#define _EMBEDDED_DATABASE_I_SHEMA_H_
#include "CommonLibrary/str_t.h"
#include "ITable.h"

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
	class IShema
	{
		public:
			IShema(){}
			virtual ~IShema(){}
			virtual size_t getTableCnt() const = 0;
			virtual ITable* getTable(size_t nIndex) const = 0;
			virtual ITable* getTableByName(const CommonLib::str_t& sTableName) const = 0;
			virtual ILink* AddLink(ITable* pTableFrom, IField* pFieldFrom, ITable* pTableTo, IField* pFieldTo,  eLinkType nLinkType) = 0;
			virtual ILink* AddLink(const CommonLib::str_t& sTableFrom, const CommonLib::str_t& sFieldFrom, const CommonLib::str_t& sTableTo, const CommonLib::str_t& sFieldTo,  eLinkType nLinkType) = 0;
			virtual bool deleteLink(ILink *pLink) = 0;
			virtual size_t getLinkCnt() const = 0;
			virtual ILink* getLink(size_t nIndex) const = 0;
	};
}

#endif