#ifndef _EMBEDDED_DATABASE_PAGE_ID_H_
#define _EMBEDDED_DATABASE_PAGE_ID_H_

enum eMainDBPageID
{
	DataBaseHeader_ID,
	SchemaPage_ID,
	SchemaTablePage_ID,
	TableHeaderPage_ID,
	FieldsPage_ID,
	FieldHeaderPage_ID,
	StorageInfoPage_ID,
	BPlusTreePage_ID
};

#endif