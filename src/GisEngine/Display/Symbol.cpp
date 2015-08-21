#include "stdafx.h"
#include "Symbol.h"

namespace GisEngine
{
	namespace Display
	{

		CSymbol::CSymbol() : m_bScaleDependent(false), m_bDrawToBuffers(false)
		{

		}
		CSymbol::~CSymbol()
		{

		}

		//ISymbol
		void CSymbol::Init( IDisplay* pDisplay ) 
		{

		}
		void CSymbol::Reset()
		{

		}
		 bool CSymbol::CanDraw(CommonLib::CGeoShape* pShape) const
		 {
			 return pShape->getPointCnt() > 0;
		 }
		void CSymbol::Draw(IDisplay* pDisplay, CommonLib::CGeoShape* pShape)
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
		void CSymbol::FlushBuffers(IDisplay* pDisplay, GisCommon::ITrackCancel* trackCancel)
		{

		}
		void CSymbol::GetBoundaryRect(CommonLib::CGeoShape* pShape, IDisplay* pDisplay,  GRect &rect) const
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
		bool CSymbol::GetScaleDependent() const
		{
			return m_bScaleDependent;
		}
		void CSymbol::SetScaleDependent(bool flag) 
		{
			m_bScaleDependent = flag;
		}
		bool CSymbol::GetDrawToBuffers() const
		{
			return m_bDrawToBuffers;
		}
		void CSymbol::SetDrawToBuffers(bool flag)
		{
			m_bDrawToBuffers = flag;
		}

		//IStreamSerialize
		bool CSymbol::save(CommonLib::IWriteStream *pWriteStream) const
		{
			pWriteStream->write(m_bScaleDependent);
			pWriteStream->write(m_bDrawToBuffers);
			return true;
		}
		bool CSymbol::load(CommonLib::IReadStream* pReadStream)
		{
			SAFE_READ_RES(pReadStream, m_bScaleDependent, 1);
			SAFE_READ_RES(pReadStream, m_bDrawToBuffers, 1);
			return true;
		}


		//IXMLSerialize
		bool CSymbol::save(GisCommon::IXMLNode* pXmlNode) const
		{
			pXmlNode->AddPropertyBool(L"SD", m_bScaleDependent);
			pXmlNode->AddPropertyBool(L"DB", m_bDrawToBuffers);
			return true;
		}
		bool CSymbol::load(GisCommon::IXMLNode* pXmlNode)
		{
			m_bScaleDependent = pXmlNode->GetPropertyBool(L"SD", m_bScaleDependent);
			m_bDrawToBuffers = pXmlNode->GetPropertyBool(L"DB", m_bDrawToBuffers);
			return true;
		}
	}
}