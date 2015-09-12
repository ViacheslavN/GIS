#include "StdAfx.h"
#include "guid.h"

#ifdef _WIN32

namespace CommonLib
{

	CGuid::CGuid(const CString &gid)
	{
		GUID	_Guid;
		int		c0, c1, c2, c3, c4, c5, c6, c7;
		int		N, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11; 

		N = swscanf(gid.cwstr() + (gid.wcharAt(0) == '{' ? 1 : 0),
			L"%8x%n-%4hx%n-%4hx%n-%2x%n%2x%n-%2x%n%2x%n%2x%n%2x%n%2x%n%2x%n",
			&_Guid.Data1, &n1, &_Guid.Data2, &n2, &_Guid.Data3, &n3, 
			&c0, &n4, &c1, &n5, &c2, &n6, &c3, &n7, &c4, &n8, &c5, &n9, &c6, &n10, &c7, &n11);

		if ( N == 11  &&  n1 == 8  &&  n2 == 13  &&  n3 == 18  &&  n4 == 21  &&  n5 == 23  &&
			n6 == 26  &&  n7 == 28  &&  n8 == 30  &&  n9 == 32  &&  n10 == 34  &&  n11 == 36 )
		{
			_Guid.Data4[0] = c0;  _Guid.Data4[1] = c1;  _Guid.Data4[2] = c2;  _Guid.Data4[3] = c3;
			_Guid.Data4[4] = c4;  _Guid.Data4[5] = c5;  _Guid.Data4[6] = c6;  _Guid.Data4[7] = c7;
			m_guid = _Guid;
		}
		else
			m_guid = guid_null;
	}

	CString CGuid::toString() const
	{
		CString str;
		str.format(L"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}", 
			m_guid.Data1, m_guid.Data2, m_guid.Data3, 
			m_guid.Data4[0], m_guid.Data4[1], m_guid.Data4[2], m_guid.Data4[3], m_guid.Data4[4], 
			m_guid.Data4[5], m_guid.Data4[6], m_guid.Data4[7]);

		return str;
	}
	CString CGuid::toCompactString() const
	{
		CString str;
		str.format(L"%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x", 
			m_guid.Data1, m_guid.Data2, m_guid.Data3, 
			m_guid.Data4[0], m_guid.Data4[1], m_guid.Data4[2], m_guid.Data4[3], m_guid.Data4[4], 
			m_guid.Data4[5], m_guid.Data4[6], m_guid.Data4[7]);

		return str;
	}

}

#endif // _WIN32