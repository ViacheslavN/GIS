#pragma once

#include "CommonLibrary/general.h"



#define rcMinX rectCoords[0]
#define rcMaxX rectCoords[1]
#define rcMinY rectCoords[2]
#define rcMaxY rectCoords[3]


uint32 key4ToBits[16]={ 0x0000, 0x0008, 0x0080, 0x0088, 
	0x0800, 0x0808, 0x0880, 0x0888,
	0x8000, 0x8008, 0x8080, 0x8088,
	0x8800, 0x8808, 0x8880, 0x8888 };



static UINT toBits5[32] = {
	0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
	0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
	0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03,
	0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03,
};


struct sRectU32
{
	static const uint32 coordMax = 0xFFFFFFFF;
	mutable uint32 rectCoords[4];
	sRectU32()
	{
		 zcoords[0]=zcoords[1]=zcoords[2]=zcoords[3]=0;
		 rectCoords[0]=rectCoords[1]=rectCoords[2]=rectCoords[3]=0;
	}

	sRectU32(uint32 minX, uint32 minY, uint32 maxX, uint32 maxY)
	{
	/*	rectCoords[0] = xMin;
		rectCoords[1] = xMax;
		rectCoords[2] = yMin;
		rectCoords[3] = yMax;*/
		int shift=0;
		for(int i=0;i<4;i++)
		{
			zcoords[i]=key4ToBits[(minX>>shift)&15]|(key4ToBits[(maxX>>shift)&15]>>1)|
				(key4ToBits[(minY>>shift)&15]>>2)|(key4ToBits[(maxY>>shift)&15]>>3);
			shift+=4;
			UINT tmp=key4ToBits[(minX>>shift)&15]|(key4ToBits[(maxX>>shift)&15]>>1)|
				(key4ToBits[(minY>>shift)&15]>>2)|(key4ToBits[(maxY>>shift)&15]>>3);
			shift+=4;
			zcoords[i]|=tmp<<16;
		}

		/*int shift=0;
		uint32 rcMaxXN=~rcMaxX;
		uint32 rcMaxYN=~rcMaxY;
		for(int i=0;i<4;i++)
		{
			zcoords[i]=key4ToBits[(rcMinX>>shift)&15]|(key4ToBits[(rcMaxXN>>shift)&15]>>1)|
				(key4ToBits[(rcMinY>>shift)&15]>>2)|(key4ToBits[(rcMaxYN>>shift)&15]>>3);
			shift+=4;
			uint32 tmp=key4ToBits[(rcMinX>>shift)&15]|(key4ToBits[(rcMaxXN>>shift)&15]>>1)|
				(key4ToBits[(rcMinY>>shift)&15]>>2)|(key4ToBits[(rcMaxYN>>shift)&15]>>3);
			shift+=4;
			zcoords[i]|=tmp<<16;
		}

		*/
	}

	bool IsInRect(const CommonLib::TRect2D<uint32>& rect)
	{
		return true;
	}
	void getXY(uint32& xMin, uint32& yMin, uint32& xMax, uint32& yMax) const
	{
		rectCoords[0] = 0;
		rectCoords[1] = 0;
		rectCoords[2] = 0;
		rectCoords[3] = 0;

		unsigned char *ptr = (unsigned char *)&zcoords[0];
		for (int i = 0; i < 16; i++)
		{
			int is = (i << 1);
			unsigned char cp = ptr[i];

			int tmp1 = toBits5[cp&0x1f];
			rectCoords[0] |= tmp1 << is;
			cp >>= 1;
			tmp1 = toBits5[cp&0x1f];
			rectCoords[1] |= tmp1 << is;
			cp >>= 1;
			tmp1 = toBits5[cp&0x1f];
			rectCoords[2] |= tmp1 << is;
			cp >>= 1;
			tmp1 = toBits5[cp&0x1f];
			rectCoords[3] |= tmp1 << is;
		}        

		yMax = rectCoords[0];
		yMin= rectCoords[1];

		 xMax= rectCoords[2];
		 xMin= rectCoords[3];
	}

	bool operator < (const sRectU32 &ref) const 
	{
		for(int i=3;i>=0;i--)
		{
			if(zcoords[i]>ref.zcoords[i])
				return false;
			if(zcoords[i]<ref.zcoords[i])
				return true;
		}
		return false;
	}

	bool less (const sRectU32 &ref) const
	{
		for(int i=3;i>=0;i--)
		{
			if(zcoords[i]>ref.zcoords[i])
				return false;
			if(zcoords[i]<ref.zcoords[i])
				return true;
		}
		return false;
	}

	bool operator > (const sRectU32 &ref) const 
	{
		return !less(ref);
	}

	void setLowBits(int idx)
	{
		uint32 bitMask = 0x88888888>>(3 - (idx&3));
		int cIdx = idx >> 5;
		for(int i= 0; i< cIdx; i++)
			zcoords[i] |= bitMask;

		uint32 bit=1 << (idx&31);
		zcoords[cIdx] -= bit;
		zcoords[cIdx] |= (bitMask&(bit-1));
	}
	void clearLowBits(int idx)
	{
		uint32 bitMask = 0x88888888 >> (3 - (idx&3));
		int cIdx=idx >> 5;
		for(int i = 0;i < cIdx; i++)
			zcoords[i] &= (~bitMask);

		uint32 bit = 1<<(idx&31);
		zcoords[cIdx] |= bit;
		zcoords[cIdx] &=~(bitMask&(bit-1));
	}

	int getBit(int idx){
		return zcoords[idx>>5]>>(idx&31);
	}

	short getBits() const
	{
		return 127;
	}

	uint32 zcoords[4];
};

struct ZRectU32Comp
{
	bool LE(const sRectU32& _Left, const sRectU32& _Right) const
	{
	/*	if(_Left.zcoords[3]!=_Right.zcoords[3])
			return (_Left.zcoords[3]<_Right.zcoords[3]);
		if(_Left.zcoords[2]!=_Right.zcoords[2])
			return (_Left.zcoords[2]<_Right.zcoords[2]);
		if(_Left.zcoords[1]!=_Right.zcoords[1])
			return (_Left.zcoords[1]<_Right.zcoords[1]);
		return (_Left.zcoords[0]<_Right.zcoords[0]);*/

		for(int i=3;i>=0;i--)
		{
			if(_Left.zcoords[i]>_Right.zcoords[i])
				return false;
			if(_Left.zcoords[i]<_Right.zcoords[i])
				return true;
		}
		return false;


	}
	bool EQ(const sRectU32& _Left, const sRectU32& _Right) const
	{
		for(int i = 0;i < 4; i++)
		{
			if(_Left.zcoords[i]!=_Right.zcoords[i])
				return false;
		}
		  return true;
	}
};