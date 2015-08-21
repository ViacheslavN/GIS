#ifndef GIS_ENGINE_DISPLAY_SIMPLE_LINE_SYMBOL_
#define GIS_ENGINE_DISPLAY_SIMPLE_LINE_SYMBOL_

#include "Symbol.h"
#include "Common/GisEngineCommon.h"

namespace GisEngine
{
	namespace Display
	{

		class CSimpleLineSymbol : public CSymbol
		{
			public:

				CSimpleLineSymbol();
				CSimpleLineSymbol( const Color &color, double width, eSimpleLineStyle style = SimpleLineStyleSolid );
				virtual ~CSimpleLineSymbol();

				virtual void DrawDirectly(IDisplay* display, const GPoint* lpPoints, const int *lpPolyCounts, int nCount );
				//CSymbol
				virtual void  DrawGeometryEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount);
				virtual void  QueryBoundaryRectEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount,  GRect &rect) const;
				
				//ILineSymbol
				virtual Color  GetColor() const ;
				virtual void   SetColor(const Color &color) ;
				virtual double GetWidth() const ;  
				virtual void   SetWidth(double width) ;

				//ISimpleLineSymbol
				virtual ePenType				GetStyle() const ;
				virtual void                SetStyle( ePenType style ) ;
				virtual eCapType             GetCapType() const ;
				virtual void                SetCapType( eCapType cap ) ;
				virtual eJoinType            GetJoinType() const ;
				virtual void                SetJoinType( eJoinType join ) ;
				
				//IStreamSerialize
				virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
				virtual bool load(CommonLib::IReadStream* pReadStream);


				//IXMLSerialize
				virtual bool save(GisCommon::IXMLNode* pXmlNode) const;
				virtual bool load(GisCommon::IXMLNode* pXmlNode);
		private:

			void Draw( IGraphics *pGraphics, IDisplayTransformation* pDT, GisXYPoint *pPt, uint32 nCnt);
		private:
			CPen m_Pen;
			
			
		
		};
	}
}

#endif