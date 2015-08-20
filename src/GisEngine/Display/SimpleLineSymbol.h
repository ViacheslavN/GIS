#ifndef GIS_ENGINE_DISPLAY_SIMPLE_LINE_SYMBOL_
#define GIS_ENGINE_DISPLAY_SIMPLE_LINE_SYMBOL_

#include "Display.h"
#include "Common/GisEngineCommon.h"

namespace GisEngine
{
	namespace Display
	{

		class CSimpleLineSymbol : public ISimpleLineSymbol, public GisCommon::IStreamSerialize, public GisCommon::IXMLSerialize
		{
			public:
				//ISymbol
				virtual void Init( IDisplay* pDisplay ) ;
				virtual void Reset() ;
				virtual void Draw(IDisplay* pDisplay, CommonLib::CGeoShape* pShape) ;
				virtual void FlushBuffers(IDisplay* pDisplay, GisCommon::ITrackCancel* trackCancel);
				virtual void GetBoundaryRect(CommonLib::CGeoShape* pShape, IDisplay* pDisplay, GisBoundingBox &bbox) const ;
				virtual bool GetScaleDependent() const ;
				virtual void SetScaleDependent(bool flag) ;
				virtual void DrawDirectly(IDisplay* pDisplay, const GPoint* lpPoints, const int *lpPolyCounts, int nCount ) ;
				
				//ILineSymbol
				virtual Color  GetColor() const ;
				virtual void   SetColor(const Color &color) ;
				virtual double GetWidth() const ;  
				virtual void   SetWidth(double width) ;

				//ISimpleLineSymbol
				virtual PenType				GetStyle() const ;
				virtual void                SetStyle( PenType style ) ;
				virtual CapType             GetCapType() const ;
				virtual void                SetCapType( CapType cap ) ;
				virtual JoinType            GetJoinType() const ;
				virtual void                SetJoinType( JoinType join ) ;
				
				//IStreamSerialize
				virtual void save(CommonLib::IWriteStream *pWriteStream) const;
				virtual void load(CommonLib::IReadStream* pReadStream);


				//IXMLSerialize
				virtual void save(GisCommon::IXMLNode* pXmlNode) const;
				virtual void load(GisCommon::IXMLNode* pXmlNode);
		private:
			CPen pPen_;
			eSimpleLineStyle lineType_;
			Color color_;
			double width_;  
			JoinType joinType_;
			CapType  capType_;
			
		
		};
	}
}

#endif