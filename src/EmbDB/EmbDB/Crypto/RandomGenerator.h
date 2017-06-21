#ifndef _EMBEDDED_DATABASE_RANDOM_GENERATOR_H_
#define _EMBEDDED_DATABASE_RANDOM_GENERATOR_H_
#include "embDB.h"
 
namespace embDB
{

	class CRandomGenerator
	{
		public:

			static bool GetRandomValues(byte *pData, uint32 nSize);
	};

}

#endif