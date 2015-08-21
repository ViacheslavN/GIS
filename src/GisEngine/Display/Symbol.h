#ifndef GIS_ENGINE_DISPLAY_SYMBOL_H_
#define GIS_ENGINE_DISPLAY_SYMBOL_H_

#include "Display.h"
#include "Common/GisEngineCommon.h"

namespace GisEngine
{
	namespace Display
	{
		class CSymbol : public ISymbol, public GisCommon::IStreamSerialize, public GisCommon::IXMLSerialize
		{
			public: 

				CSymbol();
				virtual ~CSymbol();

				//ISymbol
				virtual void Init( IDisplay* pDisplay ) ;
				virtual void Reset() ;
				virtual bool CanDraw(CommonLib::CGeoShape* pShape) const;
				virtual void Draw(IDisplay* pDisplay, CommonLib::CGeoShape* pShape) ;
				virtual void FlushBuffers(IDisplay* pDisplay, GisCommon::ITrackCancel* trackCancel);
				virtual void GetBoundaryRect(CommonLib::CGeoShape* pShape, IDisplay* pDisplay, GRect &rect) const ;
				virtual bool GetScaleDependent() const ;
				virtual void SetScaleDependent(bool flag) ;
				virtual bool GetDrawToBuffers() const;
				virtual void SetDrawToBuffers(bool flag);

			    virtual void  DrawGeometryEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount) = 0;
				virtual void  QueryBoundaryRectEx(IDisplay* pDisplay, const GPoint* points, const int* polyCounts, size_t polyCount,   GRect &rect) const= 0;
			public:
				//IStreamSerialize
				virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
				virtual bool load(CommonLib::IReadStream* pReadStream);


				//IXMLSerialize
				virtual bool save(GisCommon::IXMLNode* pXmlNode) const;
				virtual bool load(GisCommon::IXMLNode* pXmlNode);
			private:
				bool m_bScaleDependent;
				bool m_bDrawToBuffers;


		};
	}
}
#endif