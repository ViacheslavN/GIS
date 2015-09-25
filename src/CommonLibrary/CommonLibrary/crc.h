#ifndef __COMMON_LIB_CRC_H_
#define __COMMON_LIB_CRC_H_
#include "general.h"

namespace CommonLib
{
	uint32 Crc32(const unsigned char * buf, size_t len);
}


#endif