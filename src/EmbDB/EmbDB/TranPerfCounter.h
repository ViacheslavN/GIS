#ifndef _EMBEDDED_DATABASE_C_TRAN_PERF_COUNTER_H_
#define _EMBEDDED_DATABASE_C_TRAN_PERF_COUNTER_H_
#include "CommonLibrary/general.h"
namespace embDB
{
	class CTranPerfCounter
	{
	public:
			CTranPerfCounter();
			~CTranPerfCounter();

			void clear();

			void ReadDBPage() {m_nPageReadFromDBStorage++;}
			void WriteDBPage(){m_nPageWriteToDBStorage++;}
			void WriteNewDBPage(){m_nPageWriteToDBStorageNew++;}
			void RemoveDBPage(){m_nRemovePageFromDBStorage++;}

			void ReadTranPage() {m_nPageReadFromTranStorage++;}
			void WriteTranPage(){m_nPageWriteToTranStorage++;}
			void ReadFromChache(){m_nPageFromChache++;}
			void AddUndoPage(){m_nAddUndoAddr++;}


			void  OutDebugInfo();
	private:
		uint64 m_nPageReadFromDBStorage;
		uint64 m_nPageWriteToDBStorage;
		uint64 m_nPageWriteToDBStorageNew;
		uint64 m_nRemovePageFromDBStorage;
		uint64 m_nPageReadFromTranStorage;
		uint64 m_nPageWriteToTranStorage;
		uint64 m_nPageFromChache;
		uint64 m_nAddUndoAddr;
		
	};
}
#endif