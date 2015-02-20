#ifndef _EMBEDDED_DATABASE_TABLE_HEADER_H_
#define _EMBEDDED_DATABASE_TABLE_HEADER_H_

#include "CommonLibrary/str_t.h"
#include "Key.h"
#include "BaseBTree.h"
#include "VariantField.h"
#include "OIDField.h"
#include "DBFieldInfo.h"
#include <vector>
#include <map>
#include "CommonLibrary/FixedMemoryStream.h"
#include "IDBTransactions.h"
#include "PageVector.h"
#include "IDBTable.h"
namespace embDB
{
	class CTable;
	class CDatabase;
	class CStorage;

	class CTableHeader
	{
	public:

	};
}
#endif