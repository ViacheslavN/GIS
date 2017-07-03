#pragma once


#include "stream.h"
#include "MemoryStream.h"
#include "blob.h"
#include "IGeoShape.h"
namespace CommonLib
{
	namespace Private
	{
		class CStreamShapeEncoder
		{
		public:
			CStreamShapeEncoder(CommonLib::alloc_t* pAlloc = nullptr);
			~CStreamShapeEncoder();

			void clear();

			bool Encode(const CBlob& blobShape, CBlob& blobEncodeShape);

			bool BeginDecode(const CBlob& blob);

			uint32 cntParts() const;
			uint32 cntPoints() const;

			uint32 GetNextPart();
			GisXYPoint GetNextPoint();





		private:
			CommonLib::alloc_t* m_pAlloc;
			CommonLib::simple_alloc_t m_alloc;
			bool m_bInit;
		};
	}


}