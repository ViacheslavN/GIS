#include "stdafx.h"
#include "GlobalParams.h"


namespace embDB
{

	CGlobalParams::CGlobalParams() : m_bCheckCRC(false)
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

 

}