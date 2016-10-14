#ifndef _COMMON_LIB_STATIC_SIGN_COMPRESSOR_H_
#define _COMMON_LIB_STATIC_SIGN_COMPRESSOR_H_
#include "compressutils.h"
#include "WriteBitStream.h"
#include "FixedMemoryStream.h"
#include "ArithmeticCoder.h"
#include "PodVector.h"
#include "NumLenCompressor.h"
namespace CommonLib
{

	//	template<class _TEncoder, class _TDecoder>
	class TStaticSignCompressor
	{
	public:
		enum eCompressType
		{
			ONE_SIGN = 0,
			COMPRESS_POS = 1,
			NO_COMPRESS =2


		};


		TStaticSignCompressor()
		{
			clear();
		}
		~TStaticSignCompressor(){}


		void clear()
		{
			m_nSigns[0] = 0;
			m_nSigns[1] = 0;
			m_compreesType = NO_COMPRESS;
			m_DataType = dtType64;
			m_bSign = false;
			m_nVecPos.clear();
		}


		void AddSymbol(bool bSign, uint32 nPos)
		{
			m_nSigns[bSign ? 1 : 0] += 1;

			if(bSign)
				m_nVecSignPos.push_back(nPos);
			else
				m_nVecUnSignPos.push_back(nPos);

		}
		/*void RemoveSymbol(bool bSign)
		{
			m_nSigns[bSign ? 1 : 0] -= 1;
		}*/
		uint32 GetCompressSize() const
		{
			if(m_nSigns[0] == 0 || m_nSigns[1] == 0)
			{
				return 1;
			}

			uint32 nByteSize = ((m_nSigns[0] + m_nSigns[1] + 7 )/8) + 1;

			uint32 nMinCount = min(m_nSigns[0], m_nSigns[1]);

			eCompressDataType type = GetCompressType(m_nSigns[0] + m_nSigns[1]);
			uint32 nBytePosCodeSize = GetLenForDiffLen(type, nMinCount + 1);

			return min(nBytePosCodeSize, nByteSize);
		}

		void BeginCompress(CommonLib::IWriteStream *pStream)
		{

			byte nFlag = 0;
			if(m_nSigns[0] == 0 || m_nSigns[1] == 0)
			{
				m_compreesType = ONE_SIGN;
				nFlag = ONE_SIGN;

				if(m_nSigns[1] != 0)
					nFlag |= (1 << 2);

				pStream->write(nFlag);
			}
			else
			{
				uint32 nByteSize = ((m_nSigns[0] + m_nSigns[1] + 7 )/8) + 1;
				uint32 nMinCount = min(m_nSigns[0], m_nSigns[1]);
				m_DataType = GetCompressType(m_nSigns[0] + m_nSigns[1]);
				uint32 nBytePosCodeSize = GetLenForDiffLen(m_DataType, nMinCount + 1) + 1;

				if(nByteSize < nBytePosCodeSize)
				{
					m_compreesType = NO_COMPRESS;
					pStream->write((byte)m_compreesType);
					m_bitWriteStream.attach(pStream, pStream->pos(), nByteSize - 1);
					pStream->seek(nByteSize - 1, CommonLib::soFromCurrent);
				}
				else
				{
					m_compreesType = COMPRESS_POS;
					nFlag = m_compreesType;
					m_bSign = false;
					if(m_nSigns[1] == nMinCount)
					{
						nFlag |= (1 << 2);
						m_bSign = true;
					}

					m_WriteStream.attach(pStream, pStream->pos(), nBytePosCodeSize);
					m_WriteStream.write(nFlag);
					WriteValue(nMinCount, m_DataType, &m_WriteStream);

					pStream->seek(nBytePosCodeSize, CommonLib::soFromCurrent);


				}
			}


		}

		void EncodeSign(bool bSign, uint32 nPos)
		{
			switch(m_compreesType)
			{
			case COMPRESS_POS:
				{
					if(bSign == m_bSign)
					{
						WriteValue(nPos, m_DataType,  &m_WriteStream);
					}
					break;
				}
			case NO_COMPRESS:
				m_bitWriteStream.writeBit(bSign);
				break;
			}
		}


		void BeginDecompress(CommonLib::IReadStream *pStream, uint32 nCount)
		{
			byte nFlag = pStream->readByte();
			m_compreesType = (eCompressType)(nFlag & 0x03);
			if(m_compreesType == ONE_SIGN)
			{
				m_bSign = nFlag & (1 << 2);
			}
			else if(m_compreesType == COMPRESS_POS)
			{

				m_bSign = nFlag & (1 << 2);
				m_DataType = GetCompressType(nCount);

				uint32 nCount = ReadValue<uint32>(m_DataType, pStream);
				m_nVecPos.reserve(nCount);
				for (size_t i = 0; i < nCount; ++i)
				{
					uint32 nPos = ReadValue<uint32>(m_DataType, pStream);
					m_nVecPos.push_back(nPos);
				}

			}
			else if(m_compreesType == NO_COMPRESS)
			{
				m_bitReadStream.attach(pStream, pStream->pos(), (nCount + 7)/8);
				pStream->seek((nCount + 7)/8, CommonLib::soFromCurrent);
			}



		}

		bool DecodeSign(uint32 nPos)
		{
			switch(m_compreesType)
			{
			case ONE_SIGN:
				return m_bSign;
				break;
			case COMPRESS_POS:
				{

					int nIndex = CommonLib::binary_search(m_nVecPos.begin(), m_nVecPos.size(), nPos);
					return nIndex != -1 ? m_bSign : !m_bSign;
					break;
				}
			case NO_COMPRESS:
				return m_bitReadStream.readBit();
				break;
			}

			assert(false);
			return false;
		}

	private:
		uint32 m_nSigns[2];
		eCompressType m_compreesType;
		FxMemoryWriteStream m_WriteStream;
		FxBitReadStream m_bitReadStream;
		FxBitWriteStream m_bitWriteStream;
		eCompressDataType m_DataType;
		bool m_bSign;

		CommonLib::TPodVector<uint32> m_nVecPos;

		CommonLib::TPodVector<uint32> m_nVecSignPos;
		CommonLib::TPodVector<uint32> m_nVecUnSignPos;
 



		typedef TNumLemCompressor<uint32, TFindMostSigBit, 32> TNumLen32;

		//	uint32 m_nFreq[257];
		//	uint32 m_nDiff;
		//CommonLib::WriteBitStream 

	};
}

#endif