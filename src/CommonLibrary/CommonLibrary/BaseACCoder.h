#ifndef _EMBEDDED_DATABASE_BASE_AC_CODER_H_
#define _EMBEDDED_DATABASE_BASE_AC_CODER_H_

namespace CommonLib
{

	template<class TCodeValue>
	class BaseACEncoder
	{
	public:
		BaseACEncoder(){}
		virtual ~BaseACEncoder(){}

		virtual bool EncodeSymbol(TCodeValue nLowCount, TCodeValue nHightCount, TCodeValue nTotalCount) = 0;
		virtual bool EncodeFinish() = 0;
	};

	template<class TCodeValue>
	class BaseACDecoder
	{
	public:
		BaseACDecoder(){}
		virtual ~BaseACDecoder(){}

		virtual void StartDecode() = 0;
		virtual void DecodeSymbol(TCodeValue SymbolLow, TCodeValue SymbolHigh,TCodeValue TotalRange) = 0;
		virtual TCodeValue GetFreq(TCodeValue nTotalCount) = 0;
	};

}
#endif