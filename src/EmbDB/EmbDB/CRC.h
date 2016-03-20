#ifndef _EMBEDDED_DATABASE_UTILS_CRC_H_
#define _EMBEDDED_DATABASE_UTILS_CRC_H_
#include "CommonLibrary/general.h"
namespace embDB
{
	uint32 Crc32(const unsigned char * buf, uint32 len);
}


#endif