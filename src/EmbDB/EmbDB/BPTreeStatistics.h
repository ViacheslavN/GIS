#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_STATISTICS_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_STATISTICS_H_
 #include "CommonLibrary/general.h"
namespace embDB
{

	class CBPTreeStatistics
	{
		public:
			CBPTreeStatistics();
			~CBPTreeStatistics();

			void CreateNode(bool bLeaf);
			void LoadNode(bool bLeaf);
			void DeleteNode(bool bLeaf);
			void SaveNode(bool bLeaf);

			uint32 GetCreateNode(bool bLeaf) const;
			uint32 GetLoadNode(bool bLeaf) const;
			uint32 GetDeleteNode(bool bLeaf) const;
			uint32 GetSaveNode(bool bLeaf) const;
			
			void Clear();



		private:

			uint32 m_nCreateLeafNodes;
			uint32 m_nCreateInnerNodes;
			uint32 m_nLoadLeafNodes;
			uint32 m_nLoadInnerNodes;
			uint32 m_nDeleteLeafNodes;
			uint32 m_nDeleteInnerNodes;
			uint32 m_nSaveLeafNodes;
			uint32 m_nSaveInnerNodes;

	};


}

#endif