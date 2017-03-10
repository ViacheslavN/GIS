# pragma once
#include "embDB.h"
#include "CommonLibrary/string.h"
namespace embDB
{
	struct ITreeStatistic;
	struct INodeStatistic;
	struct IBtreeNodeInfo;
	struct IBPTreeInfo;
	struct ICompressorInfo;


	COMMON_LIB_REFPTR_TYPEDEF(ITreeStatistic);
	COMMON_LIB_REFPTR_TYPEDEF(IBtreeNodeInfo);
	COMMON_LIB_REFPTR_TYPEDEF(ITreeStatistic);
	COMMON_LIB_REFPTR_TYPEDEF(IBPTreeInfo);

	class IBPTreeInfo
	{
	public:
		IBPTreeInfo(){}
		virtual ~IBPTreeInfo(){}


		virtual bool IsLoad() const = 0;
		virtual bool Load() = 0;
		
		virtual CommonLib::CString GetTextInfo() = 0;
		virtual CommonLib::CString  GetHTMLInfo() = 0;

		virtual uint64 GetInnerNodeCount() const = 0;
		virtual uint64 GetLeafNodeCount() const = 0;
		
		virtual IBtreeNodeInfoPtr GetNodeInfo(int64 nAddr) = 0;

	};



}