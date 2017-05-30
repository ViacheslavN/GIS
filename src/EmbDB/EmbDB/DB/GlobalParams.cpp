#include "stdafx.h"
#include "GlobalParams.h"


namespace embDB
{

	CGlobalParams::CGlobalParams() : m_bCheckCRC(false), m_bCheckPageType(false)
	{
	}
	CGlobalParams::~CGlobalParams()
	{

	}

	CGlobalParams& CGlobalParams::Instance()
	{
		static CGlobalParams gparams;
		return gparams;
	}
	void CGlobalParams::SetCheckCRC(bool bCheck)
	{
		m_bCheckCRC = bCheck;
	}
	bool CGlobalParams::GetCheckCRC() const
	{
		return m_bCheckCRC;
	}

	void  CGlobalParams::SetCheckPageType(bool bCheckPageType)
	{
		m_bCheckPageType = bCheckPageType;
	}
	bool  CGlobalParams::GetCheckPageType() const
	{
		return m_bCheckPageType;
	}

}