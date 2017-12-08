#pragma once
#include "CommonLibrary/alloc_t.h"
#include "CommonLibrary/blob.h"
 

namespace GisEngine
{
	namespace Display
	{
		class CPointBuffer
		{
		public:
			CPointBuffer(CommonLib::alloc_t* pAlloc);
			~CPointBuffer();

			GPoint *GetPoints();
			const GPoint *GetPoints() const;
			uint32 GetSize() const;
			void Reserve(uint32 nSize);
			void push_back(const GPoint& point);
		private:
			CommonLib::alloc_t* m_pAlloc;
		};
	}
}