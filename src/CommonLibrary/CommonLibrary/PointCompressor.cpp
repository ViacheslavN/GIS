#include "stdafx.h"
#include "PointCompressor.h"
namespace CommonLib
{
	CPointCompressor::CPointCompressor(CWriteMemoryStream *pCacheStreamX, CWriteMemoryStream *pCacheStreamY)
	{

	}
	CPointCompressor::~CPointCompressor()
	{

	}

	void CPointCompressor::clear()
	{
		m_SignX.clear();
		m_SignY.clear();

		m_PointX.clear();
		m_PointY.clear();
	}
}