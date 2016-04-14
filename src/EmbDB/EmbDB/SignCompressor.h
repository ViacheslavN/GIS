#ifndef _EMBEDDED_DATABASE_SIGN_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_SIGN_COMPRESSOR_H_
#include "CommonLibrary/WriteBitStream.h"
#include "CompressUtils.h"
namespace embDB
{

//	template<class _TEncoder, class _TDecoder>
	class TSignCompressor
	{
		public:
			enum eFlag
			{
				ONE_SIGN = 0,
				POS_SIGN = 1,
				NO_COMPRESS =2,
				COMPRESS = 3


			};
	 

			TSignCompressor()
			{
				clear();
			}
			~TSignCompressor(){}


			void clear()
			{
				m_nSigns[0] = 0;
				m_nSigns[1] = 0;
			}


			void AddSymbol(bool bSign)
			{
				m_nSigns[bSign ? 1 : 0] += 1;
			}
			void RemoveSymbol(bool bSign)
			{
				m_nSigns[bSign ? 1 : 0] -= 1;
			}
			uint32 GetCompressSize() const
			{
				if(m_nSigns[0] == 0 || m_nSigns[1] == 0)
				{
					return 1;
				}
				
				uint32 nByteSize = ((m_nSigns[0] + m_nSigns[1] + 7 )/8) + 1;

				uint32 nMinCount = min(m_nSigns[0], m_nSigns[1]);

				uint32 nBytePosCodeSize = GetLenForDiffLen(GetTypeFromValue(m_nSigns[0] + m_nSigns[1]), nMinCount) + 1;
				nBytePosCodeSize +=  GetLenForDiffLen(GetTypeFromValue(m_nSigns[0] + m_nSigns[1]), 1);



				return min(nBytePosCodeSize, nByteSize);
			}

		private:
			uint32 m_nSigns[2];
		//	uint32 m_nFreq[257];
		//	uint32 m_nDiff;
			//CommonLib::WriteBitStream 

	};
}

#endif