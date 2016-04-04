#include "stdafx.h"
#include "SimpleMarketSymbol.h"
#include "DisplayUtils.h"

namespace GisEngine
{
	namespace Display
	{
		CSimpleMarketSymbol::CSimpleMarketSymbol(): m_style(SimpleMarkerStyleCircle),
			m_bIsOutline(true),
			m_OutlineColor(Color::RndLineColor()),
			m_dOutlineSize(0.1)
		{
			m_nSymbolID = SimpleMarketSymbolID;
		}
		CSimpleMarketSymbol::~CSimpleMarketSymbol()
		{

		}

		void  CSimpleMarketSymbol::SetStyle(eSimpleMarkerStyle style)
		{
			m_style = style;
		}
		void  CSimpleMarketSymbol::SetOutline(bool bOutline)
		{
			m_bIsOutline = bOutline;
		}
		bool  CSimpleMarketSymbol::IsOutline() const
		{
			return m_bIsOutline;
		}
		Color CSimpleMarketSymbol:: GetOutlineColor() const
		{
			return m_OutlineColor;
		}
		void   CSimpleMarketSymbol::SetOutlineColor( const Color &color)
		{
			m_OutlineColor = color;
		}
		double  CSimpleMarketSymbol::GetOutlineSize() const
		{
			return m_dOutlineSize;
		}
		void   CSimpleMarketSymbol::SetOutlineSize(double size)
		{
			m_dOutlineSize = size;
		}
		bool CSimpleMarketSymbol::CanDraw(CommonLib::CGeoShape* pShape) const
		{
			if(pShape->getPointCnt() > 0)
			{
				if(GetSize() > 0)
				{
					if(GetColor().GetA() != Color::Transparent)
						return true;
					else
						if(m_bIsOutline)
						{
							if(m_OutlineColor.GetA() != Color::Transparent)
							{
								if(m_dOutlineSize > 0)
									return true;
							}
						}
				}
			}
			return false;
		}


		//IStreamSerialize
		bool CSimpleMarketSymbol::save(CommonLib::IWriteStream *pWriteStream) const
		{
			CommonLib::CWriteMemoryStream stream;
			if(!TSymbolBase::save(&stream))
				return false;

			 stream.write((byte)m_style);
			 stream.write(m_bIsOutline);
			 stream.write(m_dOutlineSize);
			 m_OutlineColor.save(&stream);
			 
		
		}
		bool CSimpleMarketSymbol::load(CommonLib::IReadStream* pReadStream)
		{

			CommonLib::FxMemoryReadStream stream;
			SAFE_READ(pReadStream->save_read(&stream, true))
			if(!TSymbolBase::load(&stream))
				return false;

			byte nStyle = SimpleMarkerStyleCircle;
			SAFE_READ(stream.save_read(nStyle))
			m_style = (eSimpleMarkerStyle)nStyle;
			
			SAFE_READ(stream.save_read(m_bIsOutline))
			SAFE_READ(stream.save_read(m_dOutlineSize))
			
			return m_OutlineColor.load(&stream);
		}


		//IXMLSerialize
		bool CSimpleMarketSymbol::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
				TSymbolBase::saveXML(pXmlNode);
				pXmlNode->AddPropertyInt16(L"Style", m_style);
				pXmlNode->AddPropertyBool(L"IsOutline", m_bIsOutline);
				pXmlNode->AddPropertyDouble(L"OutlineSize", m_dOutlineSize);
				m_OutlineColor.saveXML(pXmlNode);
				return true;
		}
		bool CSimpleMarketSymbol::load(const GisCommon::IXMLNode* pXmlNode)
		{

			TSymbolBase::load(pXmlNode);
			m_style = (eSimpleMarkerStyle)pXmlNode->GetPropertyInt16(L"Style", m_style);
			m_bIsOutline = pXmlNode->GetPropertyBool(L"IsOutline", m_bIsOutline);
			m_dOutlineSize = pXmlNode->GetPropertyDouble(L"OutlineSize", m_dOutlineSize);
			m_OutlineColor.load(pXmlNode);
			return true;
		}

		void  CSimpleMarketSymbol::DrawDirectly(IDisplay* display, const GPoint* lpPoints, const int *lpPolyCounts, int nCount )
		{
			DrawGeometryEx(display, lpPoints, lpPolyCounts, nCount);
		}
		void  CSimpleMarketSymbol::DrawGeometryEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount)
		{
			for(size_t part = 0, offset = 0; part < polyCount; part++)
			{
				for(int point = (int)offset; point < (int)offset + polyCounts[part]; point++)
				{
					switch(m_style)
					{
					case SimpleMarkerStyleCircle:
						DrawCircle(points[point], pDisplay->GetGraphics().get());
						break;
					case SimpleMarkerStyleSquare:
						DrawSquare(points[point], pDisplay->GetGraphics().get());
						break;
					case SimpleMarkerStyleCross:
						DrawCross(points[point], pDisplay->GetGraphics().get());
						break;
					case SimpleMarkerStyleX:
						DrawX(points[point], pDisplay->GetGraphics().get());
						break;
					case SimpleMarkerStyleDiamond:
						DrawDiamond(points[point], pDisplay->GetGraphics().get());
						break;
					}
				}
				offset += polyCounts[part];
			}
		}
		void  CSimpleMarketSymbol::QueryBoundaryRectEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount,   GRect &rect) const
		{
	 
			for(size_t part = 0, offset = 0; part < polyCount; part++)
			{
				for(size_t p = offset; p < offset + (size_t)polyCounts[part]; p ++)
				{
					GRect r;
					r.xMin = GUnits(points[p].x - m_dDeviceSize + m_dDeviceOffsetX);
					r.xMax = GUnits(points[p].x + m_dDeviceSize + m_dDeviceOffsetX);
					r.yMin = GUnits(points[p].y - m_dDeviceSize + m_dDeviceOffsetY);
					r.yMax = GUnits(points[p].y + m_dDeviceSize + m_dDeviceOffsetY);

					r.xMax -= m_outlinePen.getWidth() / 2;
					r.yMin -= m_outlinePen.getWidth() / 2;
					r.xMax += m_outlinePen.getWidth() / 2;
					r.yMax += m_outlinePen.getWidth() / 2;

					rect.expandRect(r);
				}
			}
		}
		void  CSimpleMarketSymbol::Prepare(IDisplay* pDisplay)
		{
			TSymbolBase::Prepare(pDisplay);
			GUnits devicesizeOutline = SymbolSizeToDeviceSize(pDisplay->GetTransformation().get(), m_dOutlineSize, GetScaleDependent());

			if(m_style == SimpleMarkerStyleX || m_style == SimpleMarkerStyleCross)
			{
				m_pen.setColor(GetColor());
				if(m_bIsOutline)
				{
					m_outlinePen.setWidth(m_pen.getWidth() + 2 * devicesizeOutline);
					m_outlinePen.setColor(m_OutlineColor);
				}
			}
			else
			{
				m_brush.setColor(GetColor());
				if(m_bIsOutline)
				{
					m_outlinePen.setWidth(devicesizeOutline);
					m_outlinePen.setColor(m_OutlineColor);
				}
				else
					m_outlinePen.setPenType(PenTypeNull);
			}
		}

		void CSimpleMarketSymbol::DrawCircle(const GPoint& point, IGraphics* pGraphics)
		{
			pGraphics->DrawEllipse(&m_outlinePen, &m_brush,
				point.x - m_dDeviceSize + m_dDeviceOffsetX,
				point.y - m_dDeviceSize + m_dDeviceOffsetY,
				point.x + m_dDeviceSize + m_dDeviceOffsetX,
				point.y + m_dDeviceSize + m_dDeviceOffsetY);
		}
		void CSimpleMarketSymbol::DrawSquare(const GPoint& point, IGraphics* pGraphics)
		{
			GPoint coord[5];
			coord[0].x = point.x - m_dDeviceSize + m_dDeviceOffsetX;
			coord[0].y = point.y - m_dDeviceSize + m_dDeviceOffsetY;
			coord[1].x = point.x + m_dDeviceSize + m_dDeviceOffsetX;
			coord[1].y = point.y - m_dDeviceSize + m_dDeviceOffsetY;
			coord[2].x = point.x + m_dDeviceSize + m_dDeviceOffsetX;
			coord[2].y = point.y + m_dDeviceSize + m_dDeviceOffsetY;
			coord[3].x = point.x - m_dDeviceSize + m_dDeviceOffsetX;
			coord[3].y = point.y + m_dDeviceSize + m_dDeviceOffsetY;
			coord[4].x = point.x - m_dDeviceSize + m_dDeviceOffsetX;
			coord[4].y = point.y - m_dDeviceSize + m_dDeviceOffsetY;
			RotateCoords(point, m_dDisplayAngle, coord, 5);
			pGraphics->DrawPolygon(&m_outlinePen, &m_brush, coord, 5);
		}
		void CSimpleMarketSymbol::DrawDiamond(const GPoint& point, IGraphics* pGraphics)
		{
			GUnits centerX = point.x + m_dDeviceOffsetX;
			GUnits centerY = point.y + m_dDeviceOffsetY;

			GPoint coord[5];
			coord[0].x = centerX;
			coord[0].y = point.y - m_dDeviceSize + m_dDeviceOffsetY;

			coord[1].x = point.x - m_dDeviceSize + m_dDeviceOffsetX;
			coord[1].y = centerY;

			coord[2].x = centerX;
			coord[2].y = point.y + m_dDeviceSize + m_dDeviceOffsetY;

			coord[3].x = point.x + m_dDeviceSize + m_dDeviceOffsetX;
			coord[3].y = centerY;

			coord[4].x = centerX;
			coord[4].y = point.y - m_dDeviceSize + m_dDeviceOffsetY;

			RotateCoords(point, m_dDisplayAngle, coord, 5);

			pGraphics->DrawPolygon(&m_outlinePen, &m_brush, coord, 5);
		}
		void CSimpleMarketSymbol::DrawX(const GPoint& point, IGraphics* pGraphics)
		{
			GPoint coord[4];
			coord[0].x = point.x - m_dDeviceSize + m_dDeviceOffsetX;
			coord[0].y = point.y - m_dDeviceSize + m_dDeviceOffsetY;

			coord[1].x = point.x + m_dDeviceSize + 1 + m_dDeviceOffsetX;
			coord[1].y = point.y + m_dDeviceSize + 1 + m_dDeviceOffsetY;

			coord[2].x = point.x - m_dDeviceSize + m_dDeviceOffsetX;
			coord[2].y = point.y + m_dDeviceSize + m_dDeviceOffsetY;

			coord[3].x = point.x + m_dDeviceSize + m_dDeviceOffsetX;
			coord[3].y = point.y - m_dDeviceSize + m_dDeviceOffsetY;

			GUnits centerX = point.x + m_dDeviceOffsetX;
			GUnits centerY = point.y + m_dDeviceOffsetY;

			RotateCoords(point, m_dDisplayAngle, coord, 4);

			if (m_bIsOutline)
			{ 
				pGraphics->DrawLineSeg(&m_outlinePen, coord[0], coord[1]);    
				pGraphics->DrawLineSeg(&m_outlinePen, coord[2].x, coord[2].y,centerX, centerY);
				pGraphics->DrawLineSeg(&m_outlinePen, coord[3].x, coord[3].y,centerX, centerY);
			}

			pGraphics->DrawLineSeg(&m_pen, coord[0], coord[1]);
			pGraphics->DrawLineSeg(&m_pen, coord[2].x, coord[2].y,centerX, centerY);
			pGraphics->DrawLineSeg(&m_pen, coord[3].x, coord[3].y,centerX, centerY);            
		}
		void CSimpleMarketSymbol::DrawCross(const GPoint& point, IGraphics* pGraphics)
		{
			GPoint coord[4];
			coord[0].x = point.x + m_dDeviceOffsetX;
			coord[0].y = point.y - m_dDeviceSize + m_dDeviceOffsetY;

			coord[1].x = point.x + m_dDeviceOffsetX;
			coord[1].y = point.y + m_dDeviceSize + 1 + m_dDeviceOffsetY;

			coord[2].x = point.x - m_dDeviceSize + m_dDeviceOffsetX;
			coord[2].y = point.y + m_dDeviceOffsetY;

			coord[3].x = point.x + m_dDeviceSize + m_dDeviceOffsetX;
			coord[3].y = point.y + m_dDeviceOffsetY;

			GUnits centerX = point.x + m_dDeviceOffsetX;
			GUnits centerY = point.y + m_dDeviceOffsetY;

			RotateCoords(point, m_dDisplayAngle, coord, 4);

			if (m_bIsOutline)
			{ 
				pGraphics->DrawLineSeg(&m_outlinePen, coord[0], coord[1]);
				pGraphics->DrawLineSeg(&m_outlinePen, coord[2].x, coord[2].y,centerX,centerY);
				pGraphics->DrawLineSeg(&m_outlinePen, coord[3].x, coord[3].y,centerX,centerY);
			}

			pGraphics->DrawLineSeg(&m_outlinePen, coord[0], coord[1]);
			pGraphics->DrawLineSeg(&m_outlinePen, coord[2].x, coord[2].y,centerX,centerY);
			pGraphics->DrawLineSeg(&m_outlinePen, coord[3].x, coord[3].y,centerX,centerY);
		}
	}
}