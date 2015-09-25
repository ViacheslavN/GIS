#include "stdafx.h"
#include "SimpleLineSymbol.h"

namespace GisEngine
{
	namespace Display
	{
		CSimpleLineSymbol::CSimpleLineSymbol()
		{
			 m_nSymbolID = SimpleLineSymbolID;
		}
		CSimpleLineSymbol::CSimpleLineSymbol( const Color &color, double width, eSimpleLineStyle style)
		{
			m_Pen.setColor(color);
			m_Pen.setWidth(width);
			m_Pen.setPenType(LineStyle2PenType( style));

			m_nSymbolID = SimpleLineSymbolID;
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
			CommonLib::MemoryStream stream;
			if(!TBase::save(&stream))
				return false;

			m_Pen.save(&stream);
			pWriteStream->write(&stream);
			return true;
		}
		bool CSimpleLineSymbol::load(CommonLib::IReadStream* pReadStream)
		{
			CommonLib::FxMemoryReadStream stream;
			pReadStream->AttachStream(&stream, pReadStream->readIntu32());
			if(!TBase::load(&stream))
				return false;

			return m_Pen.load(&stream);
		}


		//IXMLSerialize
		bool CSimpleLineSymbol::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			if(!TBase::saveXML(pXmlNode))
				return false;

			return m_Pen.saveXML(pXmlNode);
		}
		bool CSimpleLineSymbol::load(const GisCommon::IXMLNode* pXmlNode)
		{
			if(!TBase::load(pXmlNode))
				return false;

			return m_Pen.load(pXmlNode);
		}

		 
	}
}