#ifndef _LIB_COMMON_BYTE_COMPRESSOR_H_
#define _LIB_COMMON_BYTE_COMPRESSOR_H_


#include "RangeCoder.h"

namespace CommonLib
{


	class CByteCompressor
	{
		public:
			CByteCompressor()
			{

			}
			~CByteCompressor()
			{

			}

			bool encodeDynamic(byte *pBytes, uint32 nCount, CommonLib::IWriteStream *pOutStream)
			{
				uint32 CalcFreq[257];
				uint32 Freq[257];
				for(int i = 0;i < 257; i++) 
				{
					Freq[i] = i;
					CalcFreq[i] = 0;
				}

				TRangeEncoder64 encoder(pOutStream);
				for (uint32 i = 1; i < nCount; ++i)
				{
					byte ch= abs(pBytes[i] -  pBytes[i - 1]);
					CalcFreq[ch] += 1;
					encoder.EncodeSymbol(Freq[ch],Freq[ch+1], Freq[256]);

					for(int j=ch+1;j<257;j++)
						Freq[j]++;	
					if(Freq[256] >= encoder.MaxRange)
						Rescale(Freq);
				}


				return true;
			}


			bool encodeStatic(byte *pBytes, uint32 nCount, CommonLib::IWriteStream *pOutStream)
			{
				uint32 FreqPrev[257];
				uint32 Freq[257];
				for(int i = 0;i < 257; i++) 
				{
					Freq[i] = 0;
					FreqPrev[i] = 0;
				}

				TRangeEncoder64 encoder(pOutStream);
				for (uint32 i = 1; i < nCount; ++i)
				{
					byte ch= abs(pBytes[i] -  pBytes[i - 1]);
					Freq[ch] += 1;
			 
				}

				int32 nPrevF = 0;
				for(int i = 0;i < 256; i++) 
				{

					FreqPrev[i + 1] = Freq[i] + nPrevF;
					nPrevF = FreqPrev[i + 1];
				}
				for (uint32 i = 1; i < nCount; ++i)
				{
					byte ch= abs(pBytes[i] -  pBytes[i - 1]);
					encoder.EncodeSymbol(FreqPrev[ch],FreqPrev[ch+1], FreqPrev[256]);

				}


				return true;
			}

		private:

			void Rescale(uint32 *Frequency) {
				for(int i = 1; i <= 256;i++) {
					Frequency[i]/=2;
					if(Frequency[i]<=Frequency[i-1]) Frequency[i]=Frequency[i-1]+1;
				}
			}
	private:
		

	};
}

#endif