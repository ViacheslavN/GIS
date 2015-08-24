#ifndef GIS_ENGINE_DISPLAY_SYMBOL_BASE_H_
#define GIS_ENGINE_DISPLAY_SYMBOL_BASE_H_

#include "Display.h"
#include "Common/GisEngineCommon.h"





namespace GisEngine
{
	namespace Display
	{
		template< class I>
		class CSymbolBase : public I
		{
		public: 

			CSymbolBase(): m_bScaleDependent(false), m_bDrawToBuffers(false), m_nSymbolID(UndefineSymbolID),
			m_dLastScale(0.), m_dLastDpi(0.), m_dLastRotation(0.)
			{	 
				 
			}
			virtual ~CSymbolBase()
			{

			}
		
			//ISymbol
			virtual uint32 GetSymbolID() const
			{
				return m_nSymbolID;
			}
			virtual void Init( IDisplay* pDisplay )
			{
				if(m_bDirty || !equals(pDisplay->GetTransformation()->GetResolution(), m_dLastDpi) || (m_bScaleDependent && !(equals(m_dLastScale, pDisplay->GetTransformation()->GetScale()))) ||
					!equals(m_dLastRotation, pDisplay->GetTransformation()->GetRotation()))
				{
					Prepare(pDisplay);
					m_bDirty = false;
					m_dLastDpi = pDisplay->GetTransformation()->GetResolution();
					m_dLastScale = pDisplay->GetTransformation()->GetScale();
					m_dLastRotation = pDisplay->GetTransformation()->GetRotation();
				}
			}
			virtual void Reset(){}
			virtual bool CanDraw(CommonLib::CGeoShape* pShape) const
			{
				 return pShape->getPointCnt() > 0;
			}
			virtual void Draw(IDisplay* pDisplay, CommonLib::CGeoShape* pShape)
			{
				GPoint* points;
				int*    parts;
				int     count;
				pDisplay->GetTransformation()->MapToDevice(*pShape, &points, &parts, &count);
				if(count > 0)
				{
					if(m_bDrawToBuffers)
					{
						DrawGeometryEx(pDisplay, points, parts, count);
					}
					else
					{
						//TO DO Draw To Buffer
					}
				}
			}
			virtual void FlushBuffers(IDisplay* pDisplay, GisCommon::ITrackCancel* trackCancel)
			{

			}
			virtual void GetBoundaryRect(CommonLib::CGeoShape* pShape, IDisplay* pDisplay, GRect &rect) const
			{
				if(!CanDraw(pShape))
					return;

				GPoint* points;
				int*    parts;
				int     count;
				pDisplay->GetTransformation()->MapToDevice(*pShape, &points, &parts, &count);
				if(count > 0)
					QueryBoundaryRectEx(pDisplay, points, parts, count, rect);
			}
			bool GetScaleDependent() const
			{
				return m_bScaleDependent;
			}
			void SetScaleDependent(bool flag) 
			{
				m_bScaleDependent = flag;
				m_bDirty = true;
			}
			bool GetDrawToBuffers() const
			{
				return m_bDrawToBuffers;
			}
			void SetDrawToBuffers(bool flag)
			{
				m_bDrawToBuffers = flag;
				m_bDirty = true;
			}

			//IStreamSerialize
			bool save(CommonLib::IWriteStream *pWriteStream) const
			{
				pWriteStream->write(GetSymbolID());
				pWriteStream->write(m_bScaleDependent);
				pWriteStream->write(m_bDrawToBuffers);
				return true;
			}
			bool load(CommonLib::IReadStream* pReadStream)
			{
				SAFE_READ_BOOL_RES(pReadStream, m_bScaleDependent);
				SAFE_READ_BOOL_RES(pReadStream, m_bDrawToBuffers);
				return true;
			}


			//IXMLSerialize
			bool saveXML(GisCommon::IXMLNode* pXmlNode) const
			{
				pXmlNode->AddPropertyBool(L"SD", m_bScaleDependent);
				pXmlNode->AddPropertyBool(L"DB", m_bDrawToBuffers);
				return true;
			}
			bool load(GisCommon::IXMLNode* pXmlNode)
			{
				m_bScaleDependent = pXmlNode->GetPropertyBool(L"SD", m_bScaleDependent);
				m_bDrawToBuffers = pXmlNode->GetPropertyBool(L"DB", m_bDrawToBuffers);
				return true;
			}
		protected:
			bool m_bScaleDependent;
			bool m_bDrawToBuffers;
			uint32 m_nSymbolID;
			bool m_bDirty;

			double  m_dLastScale;
			double  m_dLastDpi;
			double  m_dLastRotation;


		};
	}
}
#endif