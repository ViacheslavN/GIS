#include "stdafx.h"
#include "SimpleMarketSymbol.h"


namespace GisEngine
{
	namespace Display
	{
		CSimpleMarketSymbol::CSimpleMarketSymbol()
		{
			m_nSymbolID = SimpleMarketSymbolID;
		}
		CSimpleMarketSymbol::~CSimpleMarketSymbol()
		{

		}

		void  CSimpleMarketSymbol::SetStyle(eSimpleMarkerStyle style)
		{

		}
		void  CSimpleMarketSymbol::SetOutline(bool bOutline)
		{

		}
		bool  CSimpleMarketSymbol::IsOutline() const
		{

		}
		Color CSimpleMarketSymbol:: GetOutlineColor() const
		{

		}
		void   CSimpleMarketSymbol::SetOutlineColor( const Color &color)
		{

		}
		double  CSimpleMarketSymbol::GetOutlineSize() const
		{

		}
		void   CSimpleMarketSymbol::SetOutlineSize(double size)
		{

		}


		//IStreamSerialize
		bool CSimpleMarketSymbol::save(CommonLib::IWriteStream *pWriteStream) const
		{

		}
		bool CSimpleMarketSymbol::load(CommonLib::IReadStream* pReadStream)
		{

		}


		//IXMLSerialize
		bool CSimpleMarketSymbol::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{

		}
		bool CSimpleMarketSymbol::load(const GisCommon::IXMLNode* pXmlNode)
		{
		}


		void  CSimpleMarketSymbol::DrawGeometryEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount)
		{

		}
		void  CSimpleMarketSymbol::QueryBoundaryRectEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount,   GRect &rect) const
		{

		}
		void  CSimpleMarketSymbol::Prepare(IDisplay* pDisplay)
		{

		}
	}
}