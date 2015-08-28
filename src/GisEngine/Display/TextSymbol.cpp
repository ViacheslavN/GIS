#include "stdafx.h"
#include "TextSymbol.h"
#include "DisplayUtils.h"
#include "Common/Common.h"
#include "../agg/agg_trans_affine.h"
namespace GisEngine
{
	namespace Display
	{
		CTextSymbol::CTextSymbol()
		{

		}
		CTextSymbol::~CTextSymbol()
		{

		}

		GUnits CTextSymbol::GetAngle() const
		{
			return m_Font.getOrientation();
		}
		void   CTextSymbol::SetAngle( GUnits dAngle )
		{
			setDirty(true);
			m_Font.setOrientation(dAngle);
		}
		Color  CTextSymbol::GetColor() const
		{
			return m_Font.getColor();
		}
		void   CTextSymbol::SetColor(const Color &color )
		{
			setDirty(true);
			m_Font.setColor(color);
		}
		const CFont& CTextSymbol::GetFont() const
		{
			return m_Font;
		}
		CFont& CTextSymbol::GetFont()
		{
			return m_Font;
		}
		void   CTextSymbol::SetFont(const  CFont& font )
		{
			setDirty(true);
			m_Font = font;
		}
		void CTextSymbol::GetTextSize(IDisplay* display, const CommonLib::str_t& szText, GUnits *pxSize , GUnits *pySize, GUnits* baseLine) const
		{
			display->GetGraphics()->QueryTextMetrics(&m_Font, szText.cwstr(), szText.length(), pxSize, pySize, baseLine);
		}
		GUnits CTextSymbol::GetSize() const
		{
			return m_Font.getSize();
		}
		void CTextSymbol::SetSize(GUnits size)
		{
			setDirty(true);
			m_Font.setSize(size);
		}
		 const CommonLib::str_t& CTextSymbol::GetText() const
		 {
			 return m_sText;
		 }
		void CTextSymbol::SetText(const CommonLib::str_t& szText )
		{
			setDirty(true);
			m_sText = szText;
		}
		ITextBackgroundPtr CTextSymbol::GetTextBackground() const
		{
			return m_pTextBg;
		}
		void CTextSymbol::SetTextBackground( ITextBackground *pBg )
		{
			setDirty(true);
			m_pTextBg = pBg;
		}
		int   CTextSymbol::GetTextDrawFlags() const
		{
			return m_nTextDrawFlags;
		}
		void  CTextSymbol::SetTextDrawFlags(int nFlags)
		{
			setDirty(true);
			m_nTextDrawFlags = nFlags;
		}

		bool CTextSymbol::CanDraw(const CommonLib::CGeoShape* geom) const
		{
			if(m_Font.getColor().GetA() == Color::Transparent)
				return false;
			if(m_Font.getSize() == 0.0)
				return false;
			return true;
		}
		void  CTextSymbol::DrawGeometryEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount)
		{
			if(m_pGeom && m_pGeom->GetGeneralType() == CommonLib::shape_type_general_polyline)
			{
				double tmp = m_Font.getOrientation();
				m_Font.setOrientation(0);
				pDisplay->GetGraphics()->DrawTextByLine(&m_Font, m_sText.cwstr(), (int)m_sText.length(), points, polyCounts[0]);
				m_Font.setOrientation(tmp);
			}
			else
			{
				if(m_pGeom && m_pGeom->GetGeneralType() == CommonLib::shape_type_polygon)
				{
					GisXYPoint pt;
					PolygonCenterPoint(m_pGeom, &pt);

					GPoint dpt;
					pDisplay->GetTransformation()->MapToDevice(&pt, &dpt, 1);

					eTextHAlignment oldHAlignment = m_Font.getTextHAlignment();
					eTextVAlignment oldVAlignment = m_Font.getTextVAlignment();
					m_Font.setTextHAlignment(TextHAlignmentCenter);
					m_Font.setTextVAlignment(TextVAlignmentCenter);

					if(m_pTextBg.get())
						draw_background(pDisplay, dpt);
					pDisplay->GetGraphics()->DrawText(&m_Font, m_sText.cwstr(), (int)m_sText.length(), dpt, m_nTextDrawFlags);

					m_Font.setTextHAlignment(oldHAlignment);
					m_Font.setTextVAlignment(oldVAlignment);
				}
				else
				{
					draw_background(pDisplay, points[0]);
					pDisplay->GetGraphics()->DrawText(&m_Font, m_sText.cwstr(), (int)m_sText.length(), points[0], m_nTextDrawFlags);
				}
			}
		}
		void  CTextSymbol::QueryBoundaryRectEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount,   GRect &rect) const
		{
			if(polyCount > 1 || polyCounts[0] > 1)
				return;

			QueryBoundaryRectEx1(pDisplay, points[0], rect);
		}
		void  CTextSymbol::Prepare(IDisplay* pDisplay)
		{
			m_Font.setSize(SymbolSizeToDeviceSize(pDisplay->GetTransformation().get(), m_Font.getSize(), GetScaleDependent()));
			if(m_Font.getHaloSize() != 0)
				m_Font.setHaloSize(SymbolSizeToDeviceSize(pDisplay->GetTransformation().get(), m_Font.getHaloSize(), GetScaleDependent()));
		}
		void CTextSymbol::PolygonCenterPoint(const CommonLib::CGeoShape* pGeom, GisXYPoint* pout)
		{
			int k, i, j, s1, s2, MaxNodes, nPts, PtNum;
			GisXYPoint gPt;
			GisXYPoint *pNodes;
			double D, MaxD, y;

			double t;
			MaxNodes = 16;
			pNodes = new GisXYPoint[sizeof(GisXYPoint) *  MaxNodes]; //TO DO alloc
			MaxD = 0;
			const GisXYPoint* pPts = pGeom->getPoints();
			nPts = (int)pGeom->getPointCnt();
			GisBoundingBox bbox =  pGeom->getBB();
		

			GisXYPoint pVec[2];
			for(k = 1; k < 4; k++)
			{
				y = bbox.yMin + (bbox.yMax - bbox.yMin) / 4 * k;
				s1 = (pPts[0].y <= y);
				PtNum = 0;
				for(i = 1; i < nPts; i++)
				{
					s2 = (pPts[i].y <= y);
					if(s1 != s2)
					{
						if(PtNum >= MaxNodes)
						{
							MaxNodes *=2;
							delete pNodes;
							pNodes = new GisXYPoint[sizeof(GisXYPoint) *  MaxNodes];  //TO DO alloc
						}
						pNodes[PtNum].y = y;
						t = (y - pPts[i - 1].y) / (pPts[i].y - pPts[i-1].y);
						pNodes[PtNum].x = (pPts[i - 1].x + (pPts[i].x - pPts[i - 1].x) * t);
						PtNum++;
					}
					s1=s2;
				}
				for(i = 1; i < PtNum; i++)
					for(j=0; j < PtNum - i; j++)
					{
						if((pNodes[j].x <= pNodes[j + 1].x)) 
							continue;
						gPt = pNodes[j];
						pNodes[j] = pNodes[j + 1];
						pNodes[j + 1] = gPt;
					}
					for(i = 0; i < PtNum; i += 2)
					{			
						D = pNodes[i + 1].x - pNodes[i].x;
						if(D > MaxD)
						{
							pVec[0] = pNodes[i];
							pVec[1] = pNodes[i+1];
							MaxD = D;
							if (k == 2)
								MaxD += (D / 10);
						}
						else if (k == 2 && ((MaxD - D) < (D / 10)))
						{
							pVec[0] = pNodes[i];
							pVec[1] = pNodes[i + 1];
							MaxD += (D / 10);
						}
					}
			}

			if (MaxD != 0)
			{
				pout->x = pVec[0].x / 2 + pVec[1].x / 2;
				pout->y = pVec[0].y / 2 + pVec[1].y / 2;
			}
			else
				*pout = pPts[0];
		}
		void CTextSymbol::draw_background(IDisplay* pDisplay, const GPoint& pt)
		{
			if(!m_pTextBg.get())
				return;
			GRect rect;
			QueryBoundaryRectEx1(pDisplay, pt, rect);
			m_pTextBg->Draw(pDisplay, rect);
		}


		void CTextSymbol::QueryBoundaryRectEx1(IDisplay* pDisplay, const GPoint& point, GRect& rect) const
		{
			GUnits width, height, baseline;
			pDisplay->GetGraphics()->QueryTextMetrics(&m_Font, m_sText.cwstr(), (int)m_sText.length(), &width, &height, &baseline);

			switch(m_Font.getTextVAlignment())
			{
			case TextVAlignmentTop:
				rect.yMin = point.y;
				rect.yMax = point.y + height;
				break;
			case TextVAlignmentCenter:
				rect.yMin = point.y - height / 2;
				rect.yMax = point.y + height / 2;
				break;
			case TextVAlignmentBaseline:
				rect.yMin = point.y - (height - baseline);
				rect.yMax = point.y + baseline;
				break;
			case TextVAlignmentBottom:
				rect.yMin = point.y - height;
				rect.yMax = point.y;
				break;
			}

			switch(m_Font.getTextHAlignment())
			{
			case TextHAlignmentLeft:
				rect.xMin = point.x;
				rect.xMax = point.x + width;
				break;
			case TextHAlignmentCenter:
				rect.xMin = point.x - width / 2;
				rect.xMax = point.x + width / 2;
				break;
			case TextHAlignmentRight:
				rect.xMin = point.x - width;
				rect.xMax = point.x;
				break;
			}

			if(m_Font.getOrientation() != 0.0)
			{
				GisXYPoint p[4];
				p[0].x = rect.xMin;
				p[0].y = rect.yMin;
				p[1].x = rect.xMax;
				p[1].y = rect.yMin;
				p[2].x = rect.xMax;
				p[2].y = rect.yMax;
				p[3].x = rect.xMin;
				p[3].y = rect.yMax;

				agg::trans_affine mtx;
				mtx *= agg::trans_affine_translation(-point.x, -point.y);
				mtx *= agg::trans_affine_rotation(DEG2RAD(m_Font.getOrientation()));
				mtx *= agg::trans_affine_translation(point.x, point.y);
				mtx.transform(&p[0].x, &p[0].y);
				mtx.transform(&p[1].x, &p[1].y);
				mtx.transform(&p[2].x, &p[2].y);
				mtx.transform(&p[3].x, &p[3].y);

				rect.xMin = (GUnits)min(p[0].x, min(p[1].x, min(p[2].x, p[3].x)));
				rect.xMax = (GUnits)max(p[0].x, max(p[1].x, max(p[2].x, p[3].x)));
				rect.yMin = (GUnits)min(p[0].y, min(p[1].y, min(p[2].y, p[3].y)));
				rect.yMax = (GUnits)max(p[0].y, max(p[1].y, max(p[2].y, p[3].y)));
			}
		}
	}
}