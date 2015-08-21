#include "stdafx.h"
#include "SimpleLineSymbol.h"

namespace GisEngine
{
	namespace Display
	{
		CSimpleLineSymbol::CSimpleLineSymbol()
		{
			 
		}
		CSimpleLineSymbol::CSimpleLineSymbol( const Color &color, double width, eSimpleLineStyle style) 
		{
			m_Pen.setColor(color);
			m_Pen.setWidth(width);
			m_Pen.setPenType(LineStyle2PenType( style));
		}
		CSimpleLineSymbol::~CSimpleLineSymbol()
		{

		}
		void CSimpleLineSymbol::DrawDirectly(IDisplay* display, const GPoint* lpPoints, const int *lpPolyCounts, int nCount )
		{

		}
		//CSymbol
		void  CSimpleLineSymbol::DrawGeometryEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount)
		{
			IGraphicsPtr pGraphics = pDisplay->GetGraphics();
			for(int idx = 0, offset = 0; idx < (int)polyCount; ++idx)
			{
				pGraphics->DrawLine(&m_Pen, points + offset, polyCounts[idx]);
				offset += polyCounts[idx];
			}

		}
		void  CSimpleLineSymbol::QueryBoundaryRectEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount,  GRect &rect) const
		{
			for(size_t part = 0, offset = 0; part < polyCount; part++)
				for(size_t p = offset; p < offset + (size_t)polyCounts[part]; p ++)
					rect.expandRect(points[p]);

			// TODO: more accuracy calculation of boudary rect
			rect.xMin -= m_Pen.getWidth() / 2;
			rect.yMin -= m_Pen.getWidth() / 2;
			rect.xMax += m_Pen.getWidth()  / 2;
			rect.yMax += m_Pen.getWidth()  / 2;
		}

		//ILineSymbol
		Color  CSimpleLineSymbol::GetColor() const 
		{
			return m_Pen.getColor();
		}
		void   CSimpleLineSymbol::SetColor(const Color &color) 
		{
			m_Pen.setColor(color);
		}
		double CSimpleLineSymbol::GetWidth() const 
		{
			return m_Pen.getWidth();
		}
		void   CSimpleLineSymbol::SetWidth(double width) 
		{
			m_Pen.setWidth(width);
		}

		//ISimpleLineSymbol
		ePenType	CSimpleLineSymbol::GetStyle() const 
		{
			return m_Pen.getPenType();
		}
		void  CSimpleLineSymbol::SetStyle( ePenType style ) 
		{
			m_Pen.setPenType(style);
		}
		eCapType  CSimpleLineSymbol::GetCapType() const 
		{
			return m_Pen.getCapType();
		}
		void  CSimpleLineSymbol::SetCapType( eCapType cap ) 
		{
			m_Pen.setCapType(cap);
		}
		eJoinType    CSimpleLineSymbol::GetJoinType() const 
		{
			return m_Pen.getJoinType();
		}
		void  CSimpleLineSymbol::SetJoinType( eJoinType join ) 
		{
			return m_Pen.setJoinType(join);
		}

		//IStreamSerialize
		bool CSimpleLineSymbol::save(CommonLib::IWriteStream *pWriteStream) const
		{
			if(!CSymbol::save(pWriteStream))
				return false;

			return m_Pen.save(pWriteStream);
		}
		bool CSimpleLineSymbol::load(CommonLib::IReadStream* pReadStream)
		{
			if(!CSymbol::load(pReadStream))
				return false;

			return m_Pen.load(pReadStream);
		}


		//IXMLSerialize
		bool CSimpleLineSymbol::save(GisCommon::IXMLNode* pXmlNode) const
		{
			if(!CSymbol::save(pXmlNode))
				return false;

			return m_Pen.save(pXmlNode);
		}
		bool CSimpleLineSymbol::load(GisCommon::IXMLNode* pXmlNode)
		{
			if(!CSymbol::load(pXmlNode))
				return false;

			return m_Pen.load(pXmlNode);
		}

		 
	}
}