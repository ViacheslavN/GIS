#ifndef _EMBEDDED_DATABASE_NODE_LOADER_H_
#define _EMBEDDED_DATABASE_NODE_LOADER_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CommonLibrary/MemoryStream.h"
#include "storage/storage.h"
#include "SimpleCompessor.h"
namespace embDB
{

	class BTreeNodeLoader
	{
		public:
			BTreeNodeLoader(CStorage* pStorage, CommonLib::alloc_t* pAlloc);
			~BTreeNodeLoader();

			template<class _TAggrKey, class _TMemSet>
			bool LoadNode(_TMemSet& Set, int64 nAddr)
			{
				CFilePage* pFilePage =  m_pStorage->getFilePage(nAddr);
				if(!pFilePage)
					return false;
				CommonLib::MemoryStream stream(m_pAlloc);
				stream.attachBuffer(pFilePage->getRowData(), pFilePage->getPageSize());
				uint16 nTypeComp = stream.readInt32();
				switch(nTypeComp)
				{
					case SIMPLE_COMPRESSOR:
						m_SimpleComp.LoadNode(Set, nAddr, stream);
						break;
				}
			
			}
			template<class _TAggrKey, class _TMemSet>
			bool WriteNode(_TMemSet& Set, int64 nAddr, CommonLib::MemoryStream& stream)
			{
				CFilePage *pFilePage = NULL;
				if(nAddr != -1)
					pFilePage = m_pStorage->getFilePage(nAddr);
				else
					pFilePage = m_pStorage->getNewPage();

				if(!pFilePage)
					return;
				CommonLib::MemoryStream stream(m_pAlloc);
				stream.write((uint16)SIMPLE_COMPRESSOR);
				m_SimpleComp.WriteNode(Set, stream);
				m_pStorage->saveFilePage(pFilePage);
			}


		

		private:
			CStorage* m_pStorage;
			CommonLib::alloc_t* m_pAlloc;
			SimpleCompressor m_SimpleComp;
	};
}
#endif