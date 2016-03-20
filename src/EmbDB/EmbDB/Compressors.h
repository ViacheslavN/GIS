#ifndef _EMBEDDED_DATABASE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "RBSet.h"
#include "BTVector.h"

namespace embDB
{
	enum eCompressorID
	{
		SIMPLE_COMPRESSOR = 1
	};
	template<class _TNodeElem, class _TBComp = TBNodeComp<_TNodeElem> >
	class NodeCompressor
	{
		public:
			typedef _TNodeElem       TNodeElem;
			typedef _TBComp		 TBComp;	
			typedef RBSet<TNodeElem, TBComp >   TMemSet;
			//typedef BNodeVector<TNodeElem, TBComp>   TVector;
	
			NodeCompressor(){}
			virtual ~NodeCompressor(){}
			
			virtual bool LoadNode(TMemSet& Set, CommonLib::FxMemoryReadStream& stream) = 0;
			//virtual bool LoadNode(TVector& vec, CommonLib::FxMemoryReadStream& stream) = 0;

			virtual bool WriteNode(TMemSet& Set, CommonLib::FxMemoryWriteStream& stream) = 0;
					

			virtual bool insert(const TNodeElem& key) = 0;
			virtual bool remove(const TNodeElem& key) = 0;
			virtual uint32 cnt_size(uint32 nCnt) = 0;
			virtual uint32 size() const = 0;
	};
}

#endif