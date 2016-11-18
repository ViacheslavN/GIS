#ifndef _COMMON_LIB_STATIC_SIGN_COMPRESSOR_H_
#define _COMMON_LIB_STATIC_SIGN_COMPRESSOR_H_
#include "compressutils.h"
#include "WriteBitStream.h"
#include "FixedMemoryStream.h"
#include "ArithmeticCoder.h"
#include "PodVector.h"
#include "NumLenCompressor.h"
#include "BitsVector.h"
namespace CommonLib
{

	//	template<class _TEncoder, class _TDecoder>
	class TStaticSignCompressor
	{
	public:
		enum eCompressType
		{
			ONE_SIGN = 0,
			POS_IN_BIT = 1,
			NO_COMPRESS_POS =2,
			COMPRESS_POS = 3


		};

		static const uint32 __nMinCountPosForCompress = 0xff;


		TStaticSignCompressor()
		{
			clear();
		}
		~TStaticSignCompressor(){}

		void InitCompress(uint32 nCount)
		{
			m_BitVecPos.reserveBits(nCount);
			m_BitVecPos.fill(false);
		}
		void clear()
		{
			m_nSigns[0] = 0;
			m_nSigns[1] = 0;
			m_compreesType = NO_COMPRESS_POS;
			m_DataType = dtType64;
			m_bSign = false;
			m_nVecPos.clear();
			m_BitVecPos.clear();
			m_NumLen16.clear();
			m_NumLen32.clear();
			m_nCalcCompressPos = -1;
		}


		void AddSymbol(bool bSign, uint32 nPos)
		{
			m_nSigns[bSign ? 1 : 0] += 1;

			m_BitVecPos.push_back(bSign ? true : false);

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
			uint32 nBytePosCodeSize = GetLenForDiffLen(type, nMinCount + 1) + 1;

			if(m_nSigns[0] + m_nSigns[1] < __nMinCountPosForCompress)
				return min(nBytePosCodeSize, nByteSize);

			uint32 nCompressSize = CalcCompressPosSize() + 1;
			

			uint32 nMinNoCompress =  min(nBytePosCodeSize, nByteSize);
			return min(nMinNoCompress, nCompressSize);
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
				uint32 nByteBitsSize = ((m_nSigns[0] + m_nSigns[1] + 7 )/8) + 1;
				uint32 nMinCount = min(m_nSigns[0], m_nSigns[1]);
				m_DataType = GetCompressType(m_nSigns[0] + m_nSigns[1]);
				uint32 nBytePosCodeSize = GetLenForDiffLen(m_DataType, nMinCount + 1) + 1;
				uint32 nCompressPosSize = 0xffffffff;

				if(m_nSigns[0] + m_nSigns[1] > __nMinCountPosForCompress)
				{
					nCompressPosSize =  CalcCompressPosSize() + 1;
				}

				if(nByteBitsSize < nBytePosCodeSize && nByteBitsSize < nCompressPosSize)
				{
					m_compreesType = POS_IN_BIT;
					pStream->write((byte)m_compreesType);
					m_bitWriteStream.attach(pStream, pStream->pos(), nByteBitsSize - 1);
					pStream->seek(nByteBitsSize - 1, CommonLib::soFromCurrent);
				}
				else if(nCompressPosSize <  nBytePosCodeSize)
				{
					m_compreesType = COMPRESS_POS;
					nFlag = m_compreesType;
					m_bSign = false;
					if(m_nSigns[1] == nMinCount)
					{
						nFlag |= (1 << 2);
						m_bSign = true;
					}

					m_WriteStream.attach(pStream, pStream->pos(), nCompressPosSize);
					m_WriteStream.write(nFlag);

					CompressPos(&m_WriteStream);


					pStream->seek(nCompressPosSize, CommonLib::soFromCurrent);
				}
				else 
				{
					m_compreesType = NO_COMPRESS_POS;
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
			case NO_COMPRESS_POS:
				{
					if(bSign == m_bSign)
					{
						WriteValue(nPos, m_DataType,  &m_WriteStream);
					}
					break;
				}
			case POS_IN_BIT:
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
			else if(m_compreesType ==  COMPRESS_POS)
			{
				m_bSign = nFlag & (1 << 2);
				m_BitVecPos.resizeBits(nCount);
				m_BitVecPos.fill(m_bSign ? false : true);
			}
			else if(m_compreesType == NO_COMPRESS_POS)
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
			else if(m_compreesType == POS_IN_BIT)
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
				return m_BitVecPos[nPos];
				break;
			case NO_COMPRESS_POS:
				{

					int nIndex = CommonLib::binary_search(m_nVecPos.begin(), m_nVecPos.size(), nPos);
					return nIndex != -1 ? m_bSign : !m_bSign;
					break;
				}
			case POS_IN_BIT:
				return m_bitReadStream.readBit();
				break;
			}

			assert(false);
			return false;
		}

		template<class TNumLen>
		uint32 CalcCompressPosSize(TNumLen& numLen) const
		{
			if(m_nCalcCompressPos != -1)
				return (uint32)m_nCalcCompressPos;

			bool bSign = false;
			if(m_nSigns[1] < m_nSigns[0])
				bSign = true;


			int32 nPrevPos = -1;
			int nCount = 0;

			for(uint32 i = 0, sz = m_BitVecPos.sizeInBits(); i < sz; ++i)
			{
				bool bBit = m_BitVecPos.getBit(i);
				if(bBit == bSign)
				{
					if(nPrevPos != -1 && i != 0)
					{
						nCount++;
						numLen.PreAddSympol(i - nPrevPos);
					}
					nPrevPos = i;
				}

			}
			m_nCalcCompressPos = numLen.GetCompressSize() +(numLen.GetBitsLen() + 7)/8;
			return m_nCalcCompressPos;
		}

		uint32 CalcCompressPosSize() const
		{
			uint32 nCompressSize = 0;
			if((m_nSigns[0] + m_nSigns[1]) < 0xFFFF - 1)
				nCompressSize = CalcCompressPosSize(m_NumLen16) + sizeof(uint16);
			else
				nCompressSize = CalcCompressPosSize(m_NumLen32) + sizeof(uint32);

			return nCompressSize;

		}



		template<class TNumLen, class Type>
		void CompressPos(TNumLen& numLen, IWriteStream *pStream) const
		{


			uint32 nBitSize = numLen.GetBitsLen();
			uint32 nByteSize = (nBitSize + 7)/8;
			
			CommonLib::FxBitWriteStream bitStream;

			bitStream.attach(pStream, pStream->pos(), nByteSize);
			pStream->seek(nByteSize, soFromCurrent);



			int32 nPrevPos = -1;
			int32 nCount = 0;

			for(uint32 i = 0, sz = m_BitVecPos.sizeInBits(); i < sz; ++i)
			{
				bool bBit = m_BitVecPos.getBit(i);
				if(bBit == m_bSign)
				{
					if(nPrevPos == -1)
					{

						pStream->write((Type)i);
						numLen.BeginCompress(pStream);
						numLen.WriteHeader(pStream);
					}
					else
					{
						nCount++;
						numLen.EncodeSymbol(i - nPrevPos, &bitStream);
					}
					nPrevPos = i;
				}

			}
			numLen.EncodeFinish();
		}

		void CompressPos(IWriteStream *pStream)
		{
			uint32 nCompressSize = 0;
			if((m_nSigns[0] + m_nSigns[1]) < 0xFFFF - 1)
				CompressPos<TNumLen16, uint16>(m_NumLen16, pStream);
			else
				CompressPos<TNumLen32, uint32>(m_NumLen32, pStream);
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
		CommonLib::CBitsVector m_BitVecPos;
 
 



		typedef TNumLemCompressor<uint32, TFindMostSigBit, 32> TNumLen32;
		typedef TNumLemCompressor<uint16, TFindMostSigBit, 16> TNumLen16;
		mutable TNumLen32 m_NumLen32;
		mutable TNumLen16 m_NumLen16;

		mutable int32 m_nCalcCompressPos;

		//	uint32 m_nFreq[257];
		//	uint32 m_nDiff;
		//CommonLib::WriteBitStream 

	};
}

#endif