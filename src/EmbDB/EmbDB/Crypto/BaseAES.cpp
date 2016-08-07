#include "stdafx.h"
#include "BaseAES.h"

namespace embDB
{

	namespace Crypto
	{


		// little-endian arrangement from big-endian's
		// based on http://www.efgh.com/software/rijndael.htm

		//  SBOX[i] == unpack32(sbox[i], sbox[i], sbox[i], sbox[i])
		//   Te0[i] == unpack32(2*sbox[i], 1*sbox[i], 1*sbox[i], 3*sbox[i])
		//  where C*sbox[i] means galois field multiplication of C and sbox[i].

		static const uint32 SBOX[256] = {
			0x63636363UL, 0x7c7c7c7cUL, 0x77777777UL, 0x7b7b7b7bUL, 0xf2f2f2f2UL,
			0x6b6b6b6bUL, 0x6f6f6f6fUL, 0xc5c5c5c5UL, 0x30303030UL, 0x01010101UL,
			0x67676767UL, 0x2b2b2b2bUL, 0xfefefefeUL, 0xd7d7d7d7UL, 0xababababUL,
			0x76767676UL, 0xcacacacaUL, 0x82828282UL, 0xc9c9c9c9UL, 0x7d7d7d7dUL,
			0xfafafafaUL, 0x59595959UL, 0x47474747UL, 0xf0f0f0f0UL, 0xadadadadUL,
			0xd4d4d4d4UL, 0xa2a2a2a2UL, 0xafafafafUL, 0x9c9c9c9cUL, 0xa4a4a4a4UL,
			0x72727272UL, 0xc0c0c0c0UL, 0xb7b7b7b7UL, 0xfdfdfdfdUL, 0x93939393UL,
			0x26262626UL, 0x36363636UL, 0x3f3f3f3fUL, 0xf7f7f7f7UL, 0xccccccccUL,
			0x34343434UL, 0xa5a5a5a5UL, 0xe5e5e5e5UL, 0xf1f1f1f1UL, 0x71717171UL,
			0xd8d8d8d8UL, 0x31313131UL, 0x15151515UL, 0x04040404UL, 0xc7c7c7c7UL,
			0x23232323UL, 0xc3c3c3c3UL, 0x18181818UL, 0x96969696UL, 0x05050505UL,
			0x9a9a9a9aUL, 0x07070707UL, 0x12121212UL, 0x80808080UL, 0xe2e2e2e2UL,
			0xebebebebUL, 0x27272727UL, 0xb2b2b2b2UL, 0x75757575UL, 0x09090909UL,
			0x83838383UL, 0x2c2c2c2cUL, 0x1a1a1a1aUL, 0x1b1b1b1bUL, 0x6e6e6e6eUL,
			0x5a5a5a5aUL, 0xa0a0a0a0UL, 0x52525252UL, 0x3b3b3b3bUL, 0xd6d6d6d6UL,
			0xb3b3b3b3UL, 0x29292929UL, 0xe3e3e3e3UL, 0x2f2f2f2fUL, 0x84848484UL,
			0x53535353UL, 0xd1d1d1d1UL, 0x00000000UL, 0xededededUL, 0x20202020UL,
			0xfcfcfcfcUL, 0xb1b1b1b1UL, 0x5b5b5b5bUL, 0x6a6a6a6aUL, 0xcbcbcbcbUL,
			0xbebebebeUL, 0x39393939UL, 0x4a4a4a4aUL, 0x4c4c4c4cUL, 0x58585858UL,
			0xcfcfcfcfUL, 0xd0d0d0d0UL, 0xefefefefUL, 0xaaaaaaaaUL, 0xfbfbfbfbUL,
			0x43434343UL, 0x4d4d4d4dUL, 0x33333333UL, 0x85858585UL, 0x45454545UL,
			0xf9f9f9f9UL, 0x02020202UL, 0x7f7f7f7fUL, 0x50505050UL, 0x3c3c3c3cUL,
			0x9f9f9f9fUL, 0xa8a8a8a8UL, 0x51515151UL, 0xa3a3a3a3UL, 0x40404040UL,
			0x8f8f8f8fUL, 0x92929292UL, 0x9d9d9d9dUL, 0x38383838UL, 0xf5f5f5f5UL,
			0xbcbcbcbcUL, 0xb6b6b6b6UL, 0xdadadadaUL, 0x21212121UL, 0x10101010UL,
			0xffffffffUL, 0xf3f3f3f3UL, 0xd2d2d2d2UL, 0xcdcdcdcdUL, 0x0c0c0c0cUL,
			0x13131313UL, 0xececececUL, 0x5f5f5f5fUL, 0x97979797UL, 0x44444444UL,
			0x17171717UL, 0xc4c4c4c4UL, 0xa7a7a7a7UL, 0x7e7e7e7eUL, 0x3d3d3d3dUL,
			0x64646464UL, 0x5d5d5d5dUL, 0x19191919UL, 0x73737373UL, 0x60606060UL,
			0x81818181UL, 0x4f4f4f4fUL, 0xdcdcdcdcUL, 0x22222222UL, 0x2a2a2a2aUL,
			0x90909090UL, 0x88888888UL, 0x46464646UL, 0xeeeeeeeeUL, 0xb8b8b8b8UL,
			0x14141414UL, 0xdedededeUL, 0x5e5e5e5eUL, 0x0b0b0b0bUL, 0xdbdbdbdbUL,
			0xe0e0e0e0UL, 0x32323232UL, 0x3a3a3a3aUL, 0x0a0a0a0aUL, 0x49494949UL,
			0x06060606UL, 0x24242424UL, 0x5c5c5c5cUL, 0xc2c2c2c2UL, 0xd3d3d3d3UL,
			0xacacacacUL, 0x62626262UL, 0x91919191UL, 0x95959595UL, 0xe4e4e4e4UL,
			0x79797979UL, 0xe7e7e7e7UL, 0xc8c8c8c8UL, 0x37373737UL, 0x6d6d6d6dUL,
			0x8d8d8d8dUL, 0xd5d5d5d5UL, 0x4e4e4e4eUL, 0xa9a9a9a9UL, 0x6c6c6c6cUL,
			0x56565656UL, 0xf4f4f4f4UL, 0xeaeaeaeaUL, 0x65656565UL, 0x7a7a7a7aUL,
			0xaeaeaeaeUL, 0x08080808UL, 0xbabababaUL, 0x78787878UL, 0x25252525UL,
			0x2e2e2e2eUL, 0x1c1c1c1cUL, 0xa6a6a6a6UL, 0xb4b4b4b4UL, 0xc6c6c6c6UL,
			0xe8e8e8e8UL, 0xddddddddUL, 0x74747474UL, 0x1f1f1f1fUL, 0x4b4b4b4bUL,
			0xbdbdbdbdUL, 0x8b8b8b8bUL, 0x8a8a8a8aUL, 0x70707070UL, 0x3e3e3e3eUL,
			0xb5b5b5b5UL, 0x66666666UL, 0x48484848UL, 0x03030303UL, 0xf6f6f6f6UL,
			0x0e0e0e0eUL, 0x61616161UL, 0x35353535UL, 0x57575757UL, 0xb9b9b9b9UL,
			0x86868686UL, 0xc1c1c1c1UL, 0x1d1d1d1dUL, 0x9e9e9e9eUL, 0xe1e1e1e1UL,
			0xf8f8f8f8UL, 0x98989898UL, 0x11111111UL, 0x69696969UL, 0xd9d9d9d9UL,
			0x8e8e8e8eUL, 0x94949494UL, 0x9b9b9b9bUL, 0x1e1e1e1eUL, 0x87878787UL,
			0xe9e9e9e9UL, 0xcecececeUL, 0x55555555UL, 0x28282828UL, 0xdfdfdfdfUL,
			0x8c8c8c8cUL, 0xa1a1a1a1UL, 0x89898989UL, 0x0d0d0d0dUL, 0xbfbfbfbfUL,
			0xe6e6e6e6UL, 0x42424242UL, 0x68686868UL, 0x41414141UL, 0x99999999UL,
			0x2d2d2d2dUL, 0x0f0f0f0fUL, 0xb0b0b0b0UL, 0x54545454UL, 0xbbbbbbbbUL,
			0x16161616UL,
		};

		static const uint32 Te0[256] = {
			0xa56363c6UL, 0x847c7cf8UL, 0x997777eeUL, 0x8d7b7bf6UL, 0x0df2f2ffUL,
			0xbd6b6bd6UL, 0xb16f6fdeUL, 0x54c5c591UL, 0x50303060UL, 0x03010102UL,
			0xa96767ceUL, 0x7d2b2b56UL, 0x19fefee7UL, 0x62d7d7b5UL, 0xe6abab4dUL,
			0x9a7676ecUL, 0x45caca8fUL, 0x9d82821fUL, 0x40c9c989UL, 0x877d7dfaUL,
			0x15fafaefUL, 0xeb5959b2UL, 0xc947478eUL, 0x0bf0f0fbUL, 0xecadad41UL,
			0x67d4d4b3UL, 0xfda2a25fUL, 0xeaafaf45UL, 0xbf9c9c23UL, 0xf7a4a453UL,
			0x967272e4UL, 0x5bc0c09bUL, 0xc2b7b775UL, 0x1cfdfde1UL, 0xae93933dUL,
			0x6a26264cUL, 0x5a36366cUL, 0x413f3f7eUL, 0x02f7f7f5UL, 0x4fcccc83UL,
			0x5c343468UL, 0xf4a5a551UL, 0x34e5e5d1UL, 0x08f1f1f9UL, 0x937171e2UL,
			0x73d8d8abUL, 0x53313162UL, 0x3f15152aUL, 0x0c040408UL, 0x52c7c795UL,
			0x65232346UL, 0x5ec3c39dUL, 0x28181830UL, 0xa1969637UL, 0x0f05050aUL,
			0xb59a9a2fUL, 0x0907070eUL, 0x36121224UL, 0x9b80801bUL, 0x3de2e2dfUL,
			0x26ebebcdUL, 0x6927274eUL, 0xcdb2b27fUL, 0x9f7575eaUL, 0x1b090912UL,
			0x9e83831dUL, 0x742c2c58UL, 0x2e1a1a34UL, 0x2d1b1b36UL, 0xb26e6edcUL,
			0xee5a5ab4UL, 0xfba0a05bUL, 0xf65252a4UL, 0x4d3b3b76UL, 0x61d6d6b7UL,
			0xceb3b37dUL, 0x7b292952UL, 0x3ee3e3ddUL, 0x712f2f5eUL, 0x97848413UL,
			0xf55353a6UL, 0x68d1d1b9UL, 0x00000000UL, 0x2cededc1UL, 0x60202040UL,
			0x1ffcfce3UL, 0xc8b1b179UL, 0xed5b5bb6UL, 0xbe6a6ad4UL, 0x46cbcb8dUL,
			0xd9bebe67UL, 0x4b393972UL, 0xde4a4a94UL, 0xd44c4c98UL, 0xe85858b0UL,
			0x4acfcf85UL, 0x6bd0d0bbUL, 0x2aefefc5UL, 0xe5aaaa4fUL, 0x16fbfbedUL,
			0xc5434386UL, 0xd74d4d9aUL, 0x55333366UL, 0x94858511UL, 0xcf45458aUL,
			0x10f9f9e9UL, 0x06020204UL, 0x817f7ffeUL, 0xf05050a0UL, 0x443c3c78UL,
			0xba9f9f25UL, 0xe3a8a84bUL, 0xf35151a2UL, 0xfea3a35dUL, 0xc0404080UL,
			0x8a8f8f05UL, 0xad92923fUL, 0xbc9d9d21UL, 0x48383870UL, 0x04f5f5f1UL,
			0xdfbcbc63UL, 0xc1b6b677UL, 0x75dadaafUL, 0x63212142UL, 0x30101020UL,
			0x1affffe5UL, 0x0ef3f3fdUL, 0x6dd2d2bfUL, 0x4ccdcd81UL, 0x140c0c18UL,
			0x35131326UL, 0x2fececc3UL, 0xe15f5fbeUL, 0xa2979735UL, 0xcc444488UL,
			0x3917172eUL, 0x57c4c493UL, 0xf2a7a755UL, 0x827e7efcUL, 0x473d3d7aUL,
			0xac6464c8UL, 0xe75d5dbaUL, 0x2b191932UL, 0x957373e6UL, 0xa06060c0UL,
			0x98818119UL, 0xd14f4f9eUL, 0x7fdcdca3UL, 0x66222244UL, 0x7e2a2a54UL,
			0xab90903bUL, 0x8388880bUL, 0xca46468cUL, 0x29eeeec7UL, 0xd3b8b86bUL,
			0x3c141428UL, 0x79dedea7UL, 0xe25e5ebcUL, 0x1d0b0b16UL, 0x76dbdbadUL,
			0x3be0e0dbUL, 0x56323264UL, 0x4e3a3a74UL, 0x1e0a0a14UL, 0xdb494992UL,
			0x0a06060cUL, 0x6c242448UL, 0xe45c5cb8UL, 0x5dc2c29fUL, 0x6ed3d3bdUL,
			0xefacac43UL, 0xa66262c4UL, 0xa8919139UL, 0xa4959531UL, 0x37e4e4d3UL,
			0x8b7979f2UL, 0x32e7e7d5UL, 0x43c8c88bUL, 0x5937376eUL, 0xb76d6ddaUL,
			0x8c8d8d01UL, 0x64d5d5b1UL, 0xd24e4e9cUL, 0xe0a9a949UL, 0xb46c6cd8UL,
			0xfa5656acUL, 0x07f4f4f3UL, 0x25eaeacfUL, 0xaf6565caUL, 0x8e7a7af4UL,
			0xe9aeae47UL, 0x18080810UL, 0xd5baba6fUL, 0x887878f0UL, 0x6f25254aUL,
			0x722e2e5cUL, 0x241c1c38UL, 0xf1a6a657UL, 0xc7b4b473UL, 0x51c6c697UL,
			0x23e8e8cbUL, 0x7cdddda1UL, 0x9c7474e8UL, 0x211f1f3eUL, 0xdd4b4b96UL,
			0xdcbdbd61UL, 0x868b8b0dUL, 0x858a8a0fUL, 0x907070e0UL, 0x423e3e7cUL,
			0xc4b5b571UL, 0xaa6666ccUL, 0xd8484890UL, 0x05030306UL, 0x01f6f6f7UL,
			0x120e0e1cUL, 0xa36161c2UL, 0x5f35356aUL, 0xf95757aeUL, 0xd0b9b969UL,
			0x91868617UL, 0x58c1c199UL, 0x271d1d3aUL, 0xb99e9e27UL, 0x38e1e1d9UL,
			0x13f8f8ebUL, 0xb398982bUL, 0x33111122UL, 0xbb6969d2UL, 0x70d9d9a9UL,
			0x898e8e07UL, 0xa7949433UL, 0xb69b9b2dUL, 0x221e1e3cUL, 0x92878715UL,
			0x20e9e9c9UL, 0x49cece87UL, 0xff5555aaUL, 0x78282850UL, 0x7adfdfa5UL,
			0x8f8c8c03UL, 0xf8a1a159UL, 0x80898909UL, 0x170d0d1aUL, 0xdabfbf65UL,
			0x31e6e6d7UL, 0xc6424284UL, 0xb86868d0UL, 0xc3414182UL, 0xb0999929UL,
			0x772d2d5aUL, 0x110f0f1eUL, 0xcbb0b07bUL, 0xfc5454a8UL, 0xd6bbbb6dUL,
			0x3a16162cUL,
		};

		//  IBOX[i] == unpack32(ibox[i], ibox[i], ibox[i], ibox[i])
		//   Td0[i] == unpack32(14*ibox[i],  9*ibox[i], 13*ibox[i], 11*ibox[i])
		//  where C*isbox[i] means galois field multiplication of C and isbox[i].

		static const uint32 IBOX[256] = {
			0x52525252UL, 0x09090909UL, 0x6a6a6a6aUL, 0xd5d5d5d5UL, 0x30303030UL,
			0x36363636UL, 0xa5a5a5a5UL, 0x38383838UL, 0xbfbfbfbfUL, 0x40404040UL,
			0xa3a3a3a3UL, 0x9e9e9e9eUL, 0x81818181UL, 0xf3f3f3f3UL, 0xd7d7d7d7UL,
			0xfbfbfbfbUL, 0x7c7c7c7cUL, 0xe3e3e3e3UL, 0x39393939UL, 0x82828282UL,
			0x9b9b9b9bUL, 0x2f2f2f2fUL, 0xffffffffUL, 0x87878787UL, 0x34343434UL,
			0x8e8e8e8eUL, 0x43434343UL, 0x44444444UL, 0xc4c4c4c4UL, 0xdedededeUL,
			0xe9e9e9e9UL, 0xcbcbcbcbUL, 0x54545454UL, 0x7b7b7b7bUL, 0x94949494UL,
			0x32323232UL, 0xa6a6a6a6UL, 0xc2c2c2c2UL, 0x23232323UL, 0x3d3d3d3dUL,
			0xeeeeeeeeUL, 0x4c4c4c4cUL, 0x95959595UL, 0x0b0b0b0bUL, 0x42424242UL,
			0xfafafafaUL, 0xc3c3c3c3UL, 0x4e4e4e4eUL, 0x08080808UL, 0x2e2e2e2eUL,
			0xa1a1a1a1UL, 0x66666666UL, 0x28282828UL, 0xd9d9d9d9UL, 0x24242424UL,
			0xb2b2b2b2UL, 0x76767676UL, 0x5b5b5b5bUL, 0xa2a2a2a2UL, 0x49494949UL,
			0x6d6d6d6dUL, 0x8b8b8b8bUL, 0xd1d1d1d1UL, 0x25252525UL, 0x72727272UL,
			0xf8f8f8f8UL, 0xf6f6f6f6UL, 0x64646464UL, 0x86868686UL, 0x68686868UL,
			0x98989898UL, 0x16161616UL, 0xd4d4d4d4UL, 0xa4a4a4a4UL, 0x5c5c5c5cUL,
			0xccccccccUL, 0x5d5d5d5dUL, 0x65656565UL, 0xb6b6b6b6UL, 0x92929292UL,
			0x6c6c6c6cUL, 0x70707070UL, 0x48484848UL, 0x50505050UL, 0xfdfdfdfdUL,
			0xededededUL, 0xb9b9b9b9UL, 0xdadadadaUL, 0x5e5e5e5eUL, 0x15151515UL,
			0x46464646UL, 0x57575757UL, 0xa7a7a7a7UL, 0x8d8d8d8dUL, 0x9d9d9d9dUL,
			0x84848484UL, 0x90909090UL, 0xd8d8d8d8UL, 0xababababUL, 0x00000000UL,
			0x8c8c8c8cUL, 0xbcbcbcbcUL, 0xd3d3d3d3UL, 0x0a0a0a0aUL, 0xf7f7f7f7UL,
			0xe4e4e4e4UL, 0x58585858UL, 0x05050505UL, 0xb8b8b8b8UL, 0xb3b3b3b3UL,
			0x45454545UL, 0x06060606UL, 0xd0d0d0d0UL, 0x2c2c2c2cUL, 0x1e1e1e1eUL,
			0x8f8f8f8fUL, 0xcacacacaUL, 0x3f3f3f3fUL, 0x0f0f0f0fUL, 0x02020202UL,
			0xc1c1c1c1UL, 0xafafafafUL, 0xbdbdbdbdUL, 0x03030303UL, 0x01010101UL,
			0x13131313UL, 0x8a8a8a8aUL, 0x6b6b6b6bUL, 0x3a3a3a3aUL, 0x91919191UL,
			0x11111111UL, 0x41414141UL, 0x4f4f4f4fUL, 0x67676767UL, 0xdcdcdcdcUL,
			0xeaeaeaeaUL, 0x97979797UL, 0xf2f2f2f2UL, 0xcfcfcfcfUL, 0xcecececeUL,
			0xf0f0f0f0UL, 0xb4b4b4b4UL, 0xe6e6e6e6UL, 0x73737373UL, 0x96969696UL,
			0xacacacacUL, 0x74747474UL, 0x22222222UL, 0xe7e7e7e7UL, 0xadadadadUL,
			0x35353535UL, 0x85858585UL, 0xe2e2e2e2UL, 0xf9f9f9f9UL, 0x37373737UL,
			0xe8e8e8e8UL, 0x1c1c1c1cUL, 0x75757575UL, 0xdfdfdfdfUL, 0x6e6e6e6eUL,
			0x47474747UL, 0xf1f1f1f1UL, 0x1a1a1a1aUL, 0x71717171UL, 0x1d1d1d1dUL,
			0x29292929UL, 0xc5c5c5c5UL, 0x89898989UL, 0x6f6f6f6fUL, 0xb7b7b7b7UL,
			0x62626262UL, 0x0e0e0e0eUL, 0xaaaaaaaaUL, 0x18181818UL, 0xbebebebeUL,
			0x1b1b1b1bUL, 0xfcfcfcfcUL, 0x56565656UL, 0x3e3e3e3eUL, 0x4b4b4b4bUL,
			0xc6c6c6c6UL, 0xd2d2d2d2UL, 0x79797979UL, 0x20202020UL, 0x9a9a9a9aUL,
			0xdbdbdbdbUL, 0xc0c0c0c0UL, 0xfefefefeUL, 0x78787878UL, 0xcdcdcdcdUL,
			0x5a5a5a5aUL, 0xf4f4f4f4UL, 0x1f1f1f1fUL, 0xddddddddUL, 0xa8a8a8a8UL,
			0x33333333UL, 0x88888888UL, 0x07070707UL, 0xc7c7c7c7UL, 0x31313131UL,
			0xb1b1b1b1UL, 0x12121212UL, 0x10101010UL, 0x59595959UL, 0x27272727UL,
			0x80808080UL, 0xececececUL, 0x5f5f5f5fUL, 0x60606060UL, 0x51515151UL,
			0x7f7f7f7fUL, 0xa9a9a9a9UL, 0x19191919UL, 0xb5b5b5b5UL, 0x4a4a4a4aUL,
			0x0d0d0d0dUL, 0x2d2d2d2dUL, 0xe5e5e5e5UL, 0x7a7a7a7aUL, 0x9f9f9f9fUL,
			0x93939393UL, 0xc9c9c9c9UL, 0x9c9c9c9cUL, 0xefefefefUL, 0xa0a0a0a0UL,
			0xe0e0e0e0UL, 0x3b3b3b3bUL, 0x4d4d4d4dUL, 0xaeaeaeaeUL, 0x2a2a2a2aUL,
			0xf5f5f5f5UL, 0xb0b0b0b0UL, 0xc8c8c8c8UL, 0xebebebebUL, 0xbbbbbbbbUL,
			0x3c3c3c3cUL, 0x83838383UL, 0x53535353UL, 0x99999999UL, 0x61616161UL,
			0x17171717UL, 0x2b2b2b2bUL, 0x04040404UL, 0x7e7e7e7eUL, 0xbabababaUL,
			0x77777777UL, 0xd6d6d6d6UL, 0x26262626UL, 0xe1e1e1e1UL, 0x69696969UL,
			0x14141414UL, 0x63636363UL, 0x55555555UL, 0x21212121UL, 0x0c0c0c0cUL,
			0x7d7d7d7dUL,
		};

		static const uint32 Td0[256] = {
			0x50a7f451UL, 0x5365417eUL, 0xc3a4171aUL, 0x965e273aUL, 0xcb6bab3bUL,
			0xf1459d1fUL, 0xab58faacUL, 0x9303e34bUL, 0x55fa3020UL, 0xf66d76adUL,
			0x9176cc88UL, 0x254c02f5UL, 0xfcd7e54fUL, 0xd7cb2ac5UL, 0x80443526UL,
			0x8fa362b5UL, 0x495ab1deUL, 0x671bba25UL, 0x980eea45UL, 0xe1c0fe5dUL,
			0x02752fc3UL, 0x12f04c81UL, 0xa397468dUL, 0xc6f9d36bUL, 0xe75f8f03UL,
			0x959c9215UL, 0xeb7a6dbfUL, 0xda595295UL, 0x2d83bed4UL, 0xd3217458UL,
			0x2969e049UL, 0x44c8c98eUL, 0x6a89c275UL, 0x78798ef4UL, 0x6b3e5899UL,
			0xdd71b927UL, 0xb64fe1beUL, 0x17ad88f0UL, 0x66ac20c9UL, 0xb43ace7dUL,
			0x184adf63UL, 0x82311ae5UL, 0x60335197UL, 0x457f5362UL, 0xe07764b1UL,
			0x84ae6bbbUL, 0x1ca081feUL, 0x942b08f9UL, 0x58684870UL, 0x19fd458fUL,
			0x876cde94UL, 0xb7f87b52UL, 0x23d373abUL, 0xe2024b72UL, 0x578f1fe3UL,
			0x2aab5566UL, 0x0728ebb2UL, 0x03c2b52fUL, 0x9a7bc586UL, 0xa50837d3UL,
			0xf2872830UL, 0xb2a5bf23UL, 0xba6a0302UL, 0x5c8216edUL, 0x2b1ccf8aUL,
			0x92b479a7UL, 0xf0f207f3UL, 0xa1e2694eUL, 0xcdf4da65UL, 0xd5be0506UL,
			0x1f6234d1UL, 0x8afea6c4UL, 0x9d532e34UL, 0xa055f3a2UL, 0x32e18a05UL,
			0x75ebf6a4UL, 0x39ec830bUL, 0xaaef6040UL, 0x069f715eUL, 0x51106ebdUL,
			0xf98a213eUL, 0x3d06dd96UL, 0xae053eddUL, 0x46bde64dUL, 0xb58d5491UL,
			0x055dc471UL, 0x6fd40604UL, 0xff155060UL, 0x24fb9819UL, 0x97e9bdd6UL,
			0xcc434089UL, 0x779ed967UL, 0xbd42e8b0UL, 0x888b8907UL, 0x385b19e7UL,
			0xdbeec879UL, 0x470a7ca1UL, 0xe90f427cUL, 0xc91e84f8UL, 0x00000000UL,
			0x83868009UL, 0x48ed2b32UL, 0xac70111eUL, 0x4e725a6cUL, 0xfbff0efdUL,
			0x5638850fUL, 0x1ed5ae3dUL, 0x27392d36UL, 0x64d90f0aUL, 0x21a65c68UL,
			0xd1545b9bUL, 0x3a2e3624UL, 0xb1670a0cUL, 0x0fe75793UL, 0xd296eeb4UL,
			0x9e919b1bUL, 0x4fc5c080UL, 0xa220dc61UL, 0x694b775aUL, 0x161a121cUL,
			0x0aba93e2UL, 0xe52aa0c0UL, 0x43e0223cUL, 0x1d171b12UL, 0x0b0d090eUL,
			0xadc78bf2UL, 0xb9a8b62dUL, 0xc8a91e14UL, 0x8519f157UL, 0x4c0775afUL,
			0xbbdd99eeUL, 0xfd607fa3UL, 0x9f2601f7UL, 0xbcf5725cUL, 0xc53b6644UL,
			0x347efb5bUL, 0x7629438bUL, 0xdcc623cbUL, 0x68fcedb6UL, 0x63f1e4b8UL,
			0xcadc31d7UL, 0x10856342UL, 0x40229713UL, 0x2011c684UL, 0x7d244a85UL,
			0xf83dbbd2UL, 0x1132f9aeUL, 0x6da129c7UL, 0x4b2f9e1dUL, 0xf330b2dcUL,
			0xec52860dUL, 0xd0e3c177UL, 0x6c16b32bUL, 0x99b970a9UL, 0xfa489411UL,
			0x2264e947UL, 0xc48cfca8UL, 0x1a3ff0a0UL, 0xd82c7d56UL, 0xef903322UL,
			0xc74e4987UL, 0xc1d138d9UL, 0xfea2ca8cUL, 0x360bd498UL, 0xcf81f5a6UL,
			0x28de7aa5UL, 0x268eb7daUL, 0xa4bfad3fUL, 0xe49d3a2cUL, 0x0d927850UL,
			0x9bcc5f6aUL, 0x62467e54UL, 0xc2138df6UL, 0xe8b8d890UL, 0x5ef7392eUL,
			0xf5afc382UL, 0xbe805d9fUL, 0x7c93d069UL, 0xa92dd56fUL, 0xb31225cfUL,
			0x3b99acc8UL, 0xa77d1810UL, 0x6e639ce8UL, 0x7bbb3bdbUL, 0x097826cdUL,
			0xf418596eUL, 0x01b79aecUL, 0xa89a4f83UL, 0x656e95e6UL, 0x7ee6ffaaUL,
			0x08cfbc21UL, 0xe6e815efUL, 0xd99be7baUL, 0xce366f4aUL, 0xd4099feaUL,
			0xd67cb029UL, 0xafb2a431UL, 0x31233f2aUL, 0x3094a5c6UL, 0xc066a235UL,
			0x37bc4e74UL, 0xa6ca82fcUL, 0xb0d090e0UL, 0x15d8a733UL, 0x4a9804f1UL,
			0xf7daec41UL, 0x0e50cd7fUL, 0x2ff69117UL, 0x8dd64d76UL, 0x4db0ef43UL,
			0x544daaccUL, 0xdf0496e4UL, 0xe3b5d19eUL, 0x1b886a4cUL, 0xb81f2cc1UL,
			0x7f516546UL, 0x04ea5e9dUL, 0x5d358c01UL, 0x737487faUL, 0x2e410bfbUL,
			0x5a1d67b3UL, 0x52d2db92UL, 0x335610e9UL, 0x1347d66dUL, 0x8c61d79aUL,
			0x7a0ca137UL, 0x8e14f859UL, 0x893c13ebUL, 0xee27a9ceUL, 0x35c961b7UL,
			0xede51ce1UL, 0x3cb1477aUL, 0x59dfd29cUL, 0x3f73f255UL, 0x79ce1418UL,
			0xbf37c773UL, 0xeacdf753UL, 0x5baafd5fUL, 0x146f3ddfUL, 0x86db4478UL,
			0x81f3afcaUL, 0x3ec468b9UL, 0x2c342438UL, 0x5f40a3c2UL, 0x72c31d16UL,
			0x0c25e2bcUL, 0x8b493c28UL, 0x41950dffUL, 0x7101a839UL, 0xdeb30c08UL,
			0x9ce4b4d8UL, 0x90c15664UL, 0x6184cb7bUL, 0x70b632d5UL, 0x745c6c48UL,
			0x4257b8d0UL,
		};

		static inline uint32
			rolbyte (uint32 const a)
		{
			return (a << 24) | (a >> 8);
		}

		static inline uint32
			rorbyte (uint32 const a)
		{
			return (a << 8) | (a >> 24);
		}

		static inline byte
			byte0 (uint32 const a)
		{
			return a & 0xff;
		}

		static inline byte
			byte1 (uint32 const a)
		{
			return (a >> 8) & 0xff;
		}

		static inline byte
			byte2 (uint32 const a)
		{
			return (a >> 16) & 0xff;
		}

		static inline byte
			byte3 (uint32 const a)
		{
			return (a >> 24) & 0xff;
		}

		static inline uint32
			unpack32 (byte const c0, byte const c1,
			byte const c2, byte const c3)
		{
			return static_cast<uint32> (c0)
				| (static_cast<uint32> (c1) <<  8)
				| (static_cast<uint32> (c2) << 16)
				| (static_cast<uint32> (c3) << 24);
		}

		static inline void
			pack32 (byte& c0, byte& c1,
			byte& c2, byte& c3, uint32 const a)
		{
			c0 = byte0 (a);
			c1 = byte1 (a);
			c2 = byte2 (a);
			c3 = byte3 (a);
		}

		static inline uint32
			subbyte (uint32 const box[],
			uint32 const t0, uint32 const t1, uint32 const t2, uint32 const t3)
		{
			return (box[byte0 (t0)] & 0x000000ffUL)
				^ (box[byte1 (t1)] & 0x0000ff00UL)
				^ (box[byte2 (t2)] & 0x00ff0000UL)
				^ (box[byte3 (t3)] & 0xff000000UL);
		}

		static inline uint32
			subbyte (uint32 const box[], uint32 const a)
		{
			return subbyte (box, a, a, a, a);
		}

		static inline uint32
			inv_mix_column (uint32 const a)
		{
			return Td0[SBOX[byte0 (a)] & 0xff]
			^ rorbyte (Td0[SBOX[byte1 (a)] & 0xff]
			^ rorbyte (Td0[SBOX[byte2 (a)] & 0xff]
			^ rorbyte (Td0[SBOX[byte3 (a)] & 0xff])));
		}

		static inline void
			enround (uint32& t0, uint32& t1, uint32& t2, uint32& t3,
			uint32 const s0, uint32 const s1, uint32 const s2, uint32 const s3,
			uint32 const *keys)
		{
			t0 = Te0[byte0 (s0)] ^ rorbyte (Te0[byte1 (s1)] ^ rorbyte (Te0[byte2 (s2)] ^ rorbyte (Te0[byte3 (s3)]))) ^ keys[0];
			t1 = Te0[byte0 (s1)] ^ rorbyte (Te0[byte1 (s2)] ^ rorbyte (Te0[byte2 (s3)] ^ rorbyte (Te0[byte3 (s0)]))) ^ keys[1];
			t2 = Te0[byte0 (s2)] ^ rorbyte (Te0[byte1 (s3)] ^ rorbyte (Te0[byte2 (s0)] ^ rorbyte (Te0[byte3 (s1)]))) ^ keys[2];
			t3 = Te0[byte0 (s3)] ^ rorbyte (Te0[byte1 (s0)] ^ rorbyte (Te0[byte2 (s1)] ^ rorbyte (Te0[byte3 (s2)]))) ^ keys[3];
		}

		static inline void
			deround (uint32& t0, uint32& t1, uint32& t2, uint32& t3,
			uint32 const s0, uint32 const s1, uint32 const s2, uint32 const s3,
			uint32 const *ikeys)
		{
			t0 = Td0[byte0 (s0)] ^ rorbyte (Td0[byte1 (s3)] ^ rorbyte (Td0[byte2 (s2)] ^ rorbyte (Td0[byte3 (s1)]))) ^ ikeys[0];
			t1 = Td0[byte0 (s1)] ^ rorbyte (Td0[byte1 (s0)] ^ rorbyte (Td0[byte2 (s3)] ^ rorbyte (Td0[byte3 (s2)]))) ^ ikeys[1];
			t2 = Td0[byte0 (s2)] ^ rorbyte (Td0[byte1 (s1)] ^ rorbyte (Td0[byte2 (s0)] ^ rorbyte (Td0[byte3 (s3)]))) ^ ikeys[2];
			t3 = Td0[byte0 (s3)] ^ rorbyte (Td0[byte1 (s2)] ^ rorbyte (Td0[byte2 (s1)] ^ rorbyte (Td0[byte3 (s0)]))) ^ ikeys[3];
		}

		CBaseAES::CBaseAES() : m_nRounds(0)
		{
	 
		}
		CBaseAES::~CBaseAES()
		{

		}
		bool CBaseAES::setEncryptKey(byte *key, uint32 nSize)
		{
			if(nSize < getKeyLength())
				return false;


			uint32 nNK = getNK();
			uint32 nNR = getNR();
			for (uint32 i = 0, j = 0; i < nNK; ++i, j += 4)
				m_keys[i] = unpack32 (key[j], key[j + 1], key[j + 2], key[j + 3]);
			schedule_encrypt_keys (nNK, nNR, m_keys);


			return true;
		}
		bool CBaseAES::setDecryptKey(byte *key, uint32 nSize)
		{
			if(nSize < getKeyLength())
				return false;

			uint32 nNK = getNK();
			uint32 nNR = getNR();

		 
			for (int i = 0, j = 0; i < nNK; ++i, j += 4)
				m_ikeys[i] = unpack32 (key[j], key[j + 1], key[j + 2], key[j + 3]);
			schedule_decrypt_keys (nNK, nNR, m_ikeys);

			return true;
		}

		void	CBaseAES::schedule_encrypt_keys (int const nk, int const nr, uint32* rk)
		{
			int const lastkey = (BLOCKSIZE / 4) * (nr + 1);
			uint32 rcon = 1U;
			for (int i = nk, j = 0; i < lastkey; ++i) {
				if (j == 0) {
					rk[i] = rk[i - nk] ^ subbyte (SBOX, rolbyte (rk[i - 1])) ^ rcon;
					rcon = (rcon << 1) ^ ((rcon & 0x80) ? 0x11b : 0);
				}
				else if (nk > 6 && j == 4) {
					rk[i] = rk[i - nk] ^ subbyte (SBOX, rk[i - 1]);
				}
				else {
					rk[i] = rk[i - nk] ^ rk[i - 1];
				}
				j = j + 1 == nk ? 0 : j + 1;
			}
			m_nRounds = nr;
		}

		void	CBaseAES::schedule_decrypt_keys (int const nk, int const nr, uint32* rk)
		{
			schedule_encrypt_keys (nk, nr, rk);
			for (int i = 0, j = (BLOCKSIZE / 4) * m_nRounds; i < j; i += 4, j -= 4) {
				std::swap (rk[i    ], rk[j    ]);
				std::swap (rk[i + 1], rk[j + 1]);
				std::swap (rk[i + 2], rk[j + 2]);
				std::swap (rk[i + 3], rk[j + 3]);
			}
			for (int i = 1; i < m_nRounds; ++i) {
				rk += 4;
				rk[0] = inv_mix_column (rk[0]);
				rk[1] = inv_mix_column (rk[1]);
				rk[2] = inv_mix_column (rk[2]);
				rk[3] = inv_mix_column (rk[3]);
			}
		}


		bool CBaseAES::encrypt(byte* pPlain, uint32 len)
		{
			for (uint32 i = 0; i <  len; i += 16) 
			{
				encryptBlock(pPlain + i, pPlain + i);
			}
			return true;
		}
		bool CBaseAES::decrypt(byte* pSecret, uint32 len)
		{
			for (uint32 i = 0; i <  len; i += 16) 
			{
				decryptBlock(pSecret + i, pSecret + i);
			}
			return true;
		}
		bool CBaseAES::encrypt(byte* pPlain, byte* pSecret, uint32 len)
		{
			for (uint32 i = 0; i <  len; i += 16) 
			{
				encryptBlock(pPlain + i, pSecret + i);
			}
			return true;
		}
		bool CBaseAES::decrypt(byte* pSecret, byte* pPlain, uint32 len)
		{
			for (uint32 i = 0; i <  len; i += 16) 
			{
				decryptBlock(pSecret + i, pPlain + i);
			}
			return true;
		}
 
		void CBaseAES::encryptBlock(byte *plain,byte *secret)
		{
			uint32 s0, s1, s2, s3, t0, t1, t2, t3;

			s0 = unpack32 (plain[ 0], plain[ 1], plain[ 2], plain[ 3]) ^ m_keys[0];
			s1 = unpack32 (plain[ 4], plain[ 5], plain[ 6], plain[ 7]) ^ m_keys[1];
			s2 = unpack32 (plain[ 8], plain[ 9], plain[10], plain[11]) ^ m_keys[2];
			s3 = unpack32 (plain[12], plain[13], plain[14], plain[15]) ^ m_keys[3];

			enround (t0, t1, t2, t3, s0, s1, s2, s3, &m_keys[ 4]);
			enround (s0, s1, s2, s3, t0, t1, t2, t3, &m_keys[ 8]);
			enround (t0, t1, t2, t3, s0, s1, s2, s3, &m_keys[12]);
			enround (s0, s1, s2, s3, t0, t1, t2, t3, &m_keys[16]);
			enround (t0, t1, t2, t3, s0, s1, s2, s3, &m_keys[20]);
			enround (s0, s1, s2, s3, t0, t1, t2, t3, &m_keys[24]);
			enround (t0, t1, t2, t3, s0, s1, s2, s3, &m_keys[28]);
			enround (s0, s1, s2, s3, t0, t1, t2, t3, &m_keys[32]);
			enround (t0, t1, t2, t3, s0, s1, s2, s3, &m_keys[36]);
			if (m_nRounds > 10) {
				enround (s0, s1, s2, s3, t0, t1, t2, t3, &m_keys[40]);
				enround (t0, t1, t2, t3, s0, s1, s2, s3, &m_keys[44]);
				if (m_nRounds > 12) {
					enround (s0, s1, s2, s3, t0, t1, t2, t3, &m_keys[48]);
					enround (t0, t1, t2, t3, s0, s1, s2, s3, &m_keys[52]);
				}
			}
			int rk = m_nRounds << 2;
			s0 = subbyte (SBOX, t0, t1, t2, t3) ^ m_keys[rk];
			s1 = subbyte (SBOX, t1, t2, t3, t0) ^ m_keys[rk + 1];
			s2 = subbyte (SBOX, t2, t3, t0, t1) ^ m_keys[rk + 2];
			s3 = subbyte (SBOX, t3, t0, t1, t2) ^ m_keys[rk + 3];

			pack32 (secret[ 0], secret[ 1], secret[ 2], secret[ 3], s0);
			pack32 (secret[ 4], secret[ 5], secret[ 6], secret[ 7], s1);
			pack32 (secret[ 8], secret[ 9], secret[10], secret[11], s2);
			pack32 (secret[12], secret[13], secret[14], secret[15], s3);
		}
		void CBaseAES::decryptBlock(byte* secret, byte*plain)
		{
			uint32 s0, s1, s2, s3, t0, t1, t2, t3;

			s0 = unpack32 (secret[ 0], secret[ 1], secret[ 2], secret[ 3]) ^ m_ikeys[0];
			s1 = unpack32 (secret[ 4], secret[ 5], secret[ 6], secret[ 7]) ^ m_ikeys[1];
			s2 = unpack32 (secret[ 8], secret[ 9], secret[10], secret[11]) ^ m_ikeys[2];
			s3 = unpack32 (secret[12], secret[13], secret[14], secret[15]) ^ m_ikeys[3];

			deround (t0, t1, t2, t3, s0, s1, s2, s3, &m_ikeys[ 4]);
			deround (s0, s1, s2, s3, t0, t1, t2, t3, &m_ikeys[ 8]);
			deround (t0, t1, t2, t3, s0, s1, s2, s3, &m_ikeys[12]);
			deround (s0, s1, s2, s3, t0, t1, t2, t3, &m_ikeys[16]);
			deround (t0, t1, t2, t3, s0, s1, s2, s3, &m_ikeys[20]);
			deround (s0, s1, s2, s3, t0, t1, t2, t3, &m_ikeys[24]);
			deround (t0, t1, t2, t3, s0, s1, s2, s3, &m_ikeys[28]);
			deround (s0, s1, s2, s3, t0, t1, t2, t3, &m_ikeys[32]);
			deround (t0, t1, t2, t3, s0, s1, s2, s3, &m_ikeys[36]);
			if (m_nRounds > 10) {
				deround (s0, s1, s2, s3, t0, t1, t2, t3, &m_ikeys[40]);
				deround (t0, t1, t2, t3, s0, s1, s2, s3, &m_ikeys[44]);
				if (m_nRounds > 12) {
					deround (s0, s1, s2, s3, t0, t1, t2, t3, &m_ikeys[48]);
					deround (t0, t1, t2, t3, s0, s1, s2, s3, &m_ikeys[52]);
				}
			}
			int rk = m_nRounds << 2;
			s0 = subbyte (IBOX, t0, t3, t2, t1) ^ m_ikeys[rk];
			s1 = subbyte (IBOX, t1, t0, t3, t2) ^ m_ikeys[rk + 1];
			s2 = subbyte (IBOX, t2, t1, t0, t3) ^ m_ikeys[rk + 2];
			s3 = subbyte (IBOX, t3, t2, t1, t0) ^ m_ikeys[rk + 3];

			pack32 (plain[ 0], plain[ 1], plain[ 2], plain[ 3], s0);
			pack32 (plain[ 4], plain[ 5], plain[ 6], plain[ 7], s1);
			pack32 (plain[ 8], plain[ 9], plain[10], plain[11], s2);
			pack32 (plain[12], plain[13], plain[14], plain[15], s3);
		}
	
	}
}