#ifndef GIS_ENGINE_DISPLAY_SYMBOL_BASE_H_
#define GIS_ENGINE_DISPLAY_SYMBOL_BASE_H_

#include "Display.h"
#include "Common/GisEngineCommon.h"
#include "CommonLibrary/MemoryStream.h"

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
					this->Prepare(pDisplay);
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
					if(!m_bDrawToBuffers)
					{
						m_pGeom = pShape;
						this->DrawGeometryEx(pDisplay, points, parts, count);
						m_pGeom = NULL;
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
					this->QueryBoundaryRectEx(pDisplay, points, parts, count, rect);
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

			void setDirty(bool  bDirty)
			{
				m_bDirty = bDirty;
			}

			//IStreamSerialize
			bool save(CommonLib::IWriteStream *pWriteStream) const
			{

				CommonLib::MemoryStream stream;

				//pWriteStream->write(GetSymbolID());
				stream.write(m_bScaleDependent);
				stream.write(m_bDrawToBuffers);

			
				pWriteStream->write(&stream);
				return true;
			}
			bool load(CommonLib::IReadStream* pReadStream)
			{
				CommonLib::FxMemoryReadStream stream;
				pReadStream->AttachStream(&stream, pReadStream->readIntu32());
				
				stream.read(m_bScaleDependent);
				stream.read(m_bDrawToBuffers);
				return true;
			}


			//IXMLSerialize
			bool saveXML(GisCommon::IXMLNode* pXmlNode) const
			{
				pXmlNode->AddPropertyInt32U(L"SymbolID", GetSymbolID());
				pXmlNode->AddPropertyBool(L"SD", m_bScaleDependent);
				pXmlNode->AddPropertyBool(L"DB", m_bDrawToBuffers);
				return true;
			}
			bool load(const GisCommon::IXMLNode* pXmlNode)
			{
				m_bScaleDependent = pXmlNode->GetPropertyBool(L"SD", m_bScaleDependent);
				m_bDrawToBuffers = pXmlNode->GetPropertyBool(L"DB", m_bDrawToBuffers);
				return true;
			}
		protected:

			const CommonLib::CGeoShape* m_pGeom;
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