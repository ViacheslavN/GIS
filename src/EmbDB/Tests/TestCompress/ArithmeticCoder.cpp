#include "stdafx.h"
#include "ArithmeticCoder.h"


// constants to split the number space of 32 bit integers
// most significant bit kept free to prevent overflows
const unsigned int g_FirstQuarter = 0x20000000;
const unsigned int g_ThirdQuarter = 0x60000000;
const unsigned int g_Half         = 0x40000000;

ArithmeticCoderC::ArithmeticCoderC()
{
	mBitCount = 0;
	mBitBuffer = 0;

	mLow = 0;
	mHigh = 0x7FFFFFFF; // just work with least significant 31 bits
	mScale = 0;

	mBuffer = 0;
	mStep = 0;
}

void ArithmeticCoderC::SetFile( fstream *file )
{
	mFile = file;
}

void ArithmeticCoderC::SetBit( const unsigned char bit )
{
	// add bit to the buffer
	mBitBuffer = (mBitBuffer << 1) | bit;
	mBitCount++;

	if(mBitCount == 8) // buffer full
	{
		// write
		mFile->write(reinterpret_cast<char*>(&mBitBuffer),sizeof(mBitBuffer));
		mBitCount = 0;
	}
}

void ArithmeticCoderC::SetBitFlush()
{
	// fill buffer with 0 up to the next byte
	while( mBitCount != 0 )
		SetBit( 0 );
}

unsigned char ArithmeticCoderC::GetBit()
{
	if(mBitCount == 0) // buffer empty
	{
		if( !( mFile->eof() ) ) // file read completely?
			mFile->read(reinterpret_cast<char*>(&mBitBuffer),sizeof(mBitBuffer));
		else
			mBitBuffer = 0; // append zeros

		mBitCount = 8;
	}

	// extract bit from buffer
	unsigned char bit = mBitBuffer >> 7;
	mBitBuffer <<= 1;
	mBitCount--;

	return bit;
}

void ArithmeticCoderC::Encode( const unsigned int low_count,
	const unsigned int high_count,
	const unsigned int total )
	// total < 2^29
{
	// partition number space into single steps
	mStep = ( mHigh - mLow + 1 ) / total; // interval open at the top => +1

	// update upper bound
	mHigh = mLow + mStep * high_count - 1; // interval open at the top => -1

	// update lower bound
	mLow = mLow + mStep * low_count;

	// apply e1/e2 mapping
	while( ( mHigh < g_Half ) || ( mLow >= g_Half ) )
	{
		if( mHigh < g_Half )
		{
			SetBit( 0 );
			mLow = mLow * 2;
			mHigh = mHigh * 2 + 1;

			// perform e3 mappings
			for(; mScale > 0; mScale-- )
				SetBit( 1 );
		}
		else if( mLow >= g_Half )
		{
			SetBit( 1 );
			mLow = 2 * ( mLow - g_Half );
			mHigh = 2 * ( mHigh - g_Half ) + 1;

			// perform e3 mappings
			for(; mScale > 0; mScale-- )
				SetBit( 0 );
		}
	}

	// e3
	while( ( g_FirstQuarter <= mLow ) && ( mHigh < g_ThirdQuarter ) )
	{
		// keep necessary e3 mappings in mind
		mScale++;
		mLow = 2 * ( mLow - g_FirstQuarter );
		mHigh = 2 * ( mHigh - g_FirstQuarter ) + 1;
	}
}

void ArithmeticCoderC::EncodeFinish()
{
	// There are two possibilities of how mLow and mHigh can be distributed,
	// which means that two bits are enough to distinguish them.

	if( mLow < g_FirstQuarter ) // mLow < FirstQuarter < Half <= mHigh
	{
		SetBit( 0 );

		for( int i=0; i<mScale+1; i++ ) // perform e3-skaling
			SetBit(1);
	}
	else // mLow < Half < ThirdQuarter <= mHigh
	{
		SetBit( 1 ); // zeros added automatically by the decoder; no need to send them
	}

	// empty the output buffer
	SetBitFlush();
}

void ArithmeticCoderC::DecodeStart()
{
	// Fill buffer with bits from the input stream
	for( int i=0; i<31; i++ ) // just use the 31 least significant bits
		mBuffer = ( mBuffer << 1 ) | GetBit();
}

unsigned int ArithmeticCoderC::DecodeTarget( const unsigned int total )
	// total < 2^29
{
	// split number space into single steps
	mStep = ( mHigh - mLow + 1 ) / total; // interval open at the top => +1

	// return current value
	return ( mBuffer - mLow ) / mStep;
}

void ArithmeticCoderC::Decode( const unsigned int low_count,
	const unsigned int high_count )
{
	// update upper bound
	mHigh = mLow + mStep * high_count - 1; // interval open at the top => -1

	// update lower bound
	mLow = mLow + mStep * low_count;

	// e1/e2 mapping
	while( ( mHigh < g_Half ) || ( mLow >= g_Half ) )
	{
		if( mHigh < g_Half )
		{
			mLow = mLow * 2;
			mHigh = mHigh * 2 + 1;
			mBuffer = 2 * mBuffer + GetBit();
		}
		else if( mLow >= g_Half )
		{
			mLow = 2 * ( mLow - g_Half );
			mHigh = 2 * ( mHigh - g_Half ) + 1;
			mBuffer = 2 * ( mBuffer - g_Half ) + GetBit();
		}
		mScale = 0;
	}

	// e3 mapping
	while( ( g_FirstQuarter <= mLow ) && ( mHigh < g_ThirdQuarter ) )
	{
		mScale++;
		mLow = 2 * ( mLow - g_FirstQuarter );
		mHigh = 2 * ( mHigh - g_FirstQuarter ) + 1;
		mBuffer = 2 * ( mBuffer - g_FirstQuarter ) + GetBit();
	}
}
