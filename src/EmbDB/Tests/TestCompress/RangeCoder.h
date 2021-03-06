#ifndef _EMBEDDED_DATABASE_RANGE_CODER_H_
#define _EMBEDDED_DATABASE_RANGE_CODER_H_
#include "CommonLibrary/MemoryStream.h"
namespace embDB
{
	template<class _TCodeValue, uint16 _nValueBits>
	class TRangeEncoder
	{
	
			typedef _TCodeValue TCodeValue;
				static const uint16 nValueBits	= _nValueBits - 8;
				static const _TCodeValue Top = (_TCodeValue)1 << nValueBits;
				static const _TCodeValue Bottom = (_TCodeValue)1 << (nValueBits - 8);
				_TCodeValue Low,Range;
	public:

				static const _TCodeValue MaxRange = Bottom;
				static const uint32 Max_Size_Error = 100; //1 %
				TRangeEncoder(CommonLib::IWriteStream* pStream) : m_pStream(pStream) ,Low(0), Range((_TCodeValue)-1)
				{

				}
				void EncodeSymbol(_TCodeValue SymbolLow, _TCodeValue SymbolHigh,_TCodeValue TotalRange)
				{

					Low += SymbolLow*(Range/=TotalRange);
					Range *= SymbolHigh-SymbolLow; 

				/*	while ((Low ^ (Low+Range))<Top || 
						Range<Bottom && 
						((Range= -Low & (Bottom-1)),1))
					{
						m_pStream->write(byte((Low>>nValueBits)& 0xFF)), 	Range<<=8,	Low<<=8; 
					}*/
					for (;;)
					{
						if ((Low ^ (Low + Range)) < Top)
						{
				
						} 
						else if (Range < Bottom)
						{  
							Range = -Low & (Bottom - 1);
						} 
						else
						{
							break;
						}
						m_pStream->write(byte((Low>>nValueBits)& 0xFF));
						Range<<=8;
						Low<<=8;  
					}


					 
				}
				void EncodeFinish()
				{
					for(int i=0;i<_nValueBits/8;i++)
					{
						m_pStream->write((byte)((Low>>nValueBits)& 0xFF));
						Low<<=8;
					}

				}
		private:
			CommonLib::IWriteStream* m_pStream;

	};



	template<class _TCodeValue, uint16 _nValueBits>
	class TRangeDecoder
	{

		static const uint16 nValueBits	= _nValueBits - 8;
		static const _TCodeValue Top = (_TCodeValue)1 << nValueBits;
		static const _TCodeValue Bottom = (_TCodeValue)1 << (nValueBits - 8);
	public:

		static const _TCodeValue MaxRange = Bottom;
	
		typedef _TCodeValue TCodeValue;

		TRangeDecoder(CommonLib::IReadStream* pStream) : m_pStream(pStream) ,Low(0), Range((_TCodeValue)-1), m_nValue(0)
		{

		}

		void StartDecode()
		{
			for(int i=0;i<_nValueBits/8;i++)
			{
				m_nValue = (m_nValue << 8) | m_pStream->readByte();
			}
		}

		void DecodeSymbol(_TCodeValue SymbolLow, _TCodeValue SymbolHigh,_TCodeValue TotalRange)
		{
			Low += SymbolLow*Range;
			Range *= SymbolHigh-SymbolLow;

			while ((Low ^ Low+Range)<Top || Range<Bottom && ((Range= -Low & Bottom-1),1))
			{
				m_nValue= m_nValue<<8 | m_pStream->readByte(), 	Range<<=8,	Low<<=8;
			}
		}

		TCodeValue GetFreq(TCodeValue nTotalCount)
		{
			return (m_nValue-Low)/(Range/=nTotalCount);
		}

		 byte ReadByte()
		 {
			 byte bb = 0;
			 m_pStream->save_read(bb);
			 return bb;
		 }
	private:
		CommonLib::IReadStream* m_pStream;

		_TCodeValue m_nValue;
		_TCodeValue Low,Range;

	};


}

#endif