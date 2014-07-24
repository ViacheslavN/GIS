#ifndef _EMBEDDED_DATABASE_FILE_PAGE_TYPE_H_
#define _EMBEDDED_DATABASE_FILE_PAGE_TYPE_H_
namespace embDB
{
	enum eObjectPageType
	{
		DATABASE_PAGE = 1,
		SCHEMA_PAGE,
		TABLE_PAGE,
		BTREE_PAGE,
		FIELD_PAGE,
		FREE_PAGE_LIST,
		STORAGE_PAGE,
		TRANSACTION_PAGE
	};
	enum eDatabasePageType
	{
		DB_HEADER_PAGE
		
	};

	enum eBtreeTypePage
	{
		BTREE_ROOT_PAGE,
		BTREE_LEAF_PAGE,
		BTREE_INNER_PAGE,
		BTREE_STATIC_PAGE,
		BTREE_INFO_PAGE
	};

	enum eTableTypePage
	{
		TABLE_HEADER_PAGE,
		TABLE_FIELD_PAGE,
		TABLE_FIELD_LIST_PAGE
	};

	enum eSchemaPageType
	{
		SCHEMA_ROOT_PAGE,
		SCHEMA_TABLE_LIST_PAGE
	};

	enum eStoragePageType
	{
		STORAGE_INFO_PAGE,
		STORAGE_LIST_FREEMAP_PAGE,
		STORAGE_FREE_MAP_PAGE
	};

	enum eFieldPageType
	{
		FIELD_INFO_PAGE
	};

	enum eTransactionsPageType
	{
		UNDO_PAGES
	};
}
#endif