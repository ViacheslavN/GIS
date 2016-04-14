#ifndef _COMMON_LIB_SIGN_COMPRESSOR_H_
#define _COMMON_LIB_SIGN_COMPRESSOR_H_
#include "compressutils.h"
#include "WriteBitStream.h"
namespace CommonLib
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

		void Init(uint32 nSize)
		{

		}


		void AddSymbol(bool bSign)
		{
			m_nSigns[bSign ? 1 : 0] += 1;

		}
		 
		uint32 GetCompressSize() const
		{
			if(m_nSigns[0] == 0 || m_nSigns[1] == 0)
			{
				return 1;
			}

			uint32 nByteSize = ((m_nSigns[0] + m_nSigns[1] + 7 )/8) + 1;

			uint32 nMinCount = min(m_nSigns[0], m_nSigns[1]);

			eCompressDataType eDataType = GetCompressType(m_nSigns[0] + m_nSigns[1]);
			uint32 nBytePosCodeSize =  GetSizeTypeValue(eDataType) * (nMinCount + 1);



			return min(nBytePosCodeSize, nByteSize);
		}


		void InitCompress(IWriteStream *pStream)
		{
			m_nFlags = 0;
			if(m_nSigns[0] == 0 || m_nSigns[1] == 0)
			{
				 m_nFlags = ONE_SIGN;
				 if(m_nSigns[1] == 0)
					 m_nFlags |= (1 << 2);
			}
			else 
			{
				uint32 nByteSize = ((m_nSigns[0] + m_nSigns[1] + 7 )/8) + 1;
				uint32 nMinCount = min(m_nSigns[0], m_nSigns[1]);
				
				eCompressDataType eDataType = GetCompressType(m_nSigns[0] + m_nSigns[1]);
				uint32 nBytePosCodeSize =  GetSizeTypeValue(eDataType) * (nMinCount + 2);
				if(nBytePosCodeSize < nByteSize)
				{
					 m_nFlags = POS_SIGN;
					 byte nComType = (byte)eDataType;

					 m_nFlags |= (nComType << 2);

				}
				else
				{

					 m_nFlags = NO_COMPRESS;
				}

			}

		//	pStream->write(m_nFlags);

		}



	private:
		uint32 m_nSigns[2];
		byte m_nFlags;
	

	};
}

#endif