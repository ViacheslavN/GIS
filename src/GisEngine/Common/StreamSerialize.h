#ifndef COMMON_GIS_ENGINE_SYSTEM_STREAMSERIALIZE_H
#define COMMON_GIS_ENGINE_SYSTEM_STREAMSERIALIZE_H

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/MemoryStream.h"
namespace GisEngine
{

	namespace Common
	{

		class StreamSerialize
		{
		public:
			StreamSerialize(void);
			virtual ~StreamSerialize(void);
		public:
			virtual void Save(CommonLib::IWriteStream *pWriteStream) const = 0;
			virtual void Load(CommonLib::IReadStream* pReadStream) = 0;
		};

	}

}

#endif