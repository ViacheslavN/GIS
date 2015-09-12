#pragma once

#ifdef _WIN32
#include <objbase.h>

#if _MSC_VER < 1300
#define _HASH_SEED (size_t)0xdeadbeef
#else
#include <xhash>
#endif

#include "String.h"

namespace CommonLib
{
	const GUID guid_null = {0x00000000L, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	const GUID guid_one  = {0x00000000L, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

	class CGuid
	{
	public:
		CGuid(void) {generate();}
		CGuid(const CGuid &gid) {m_guid = gid.m_guid;}
		CGuid(const GUID gid) {m_guid = gid;}
		CGuid(const CString &gid);

		~CGuid(void) {}
		CGuid& operator =(const CGuid& gid) {m_guid = gid.m_guid; return *this; }

		CString toString() const;
		CString toCompactString() const;
		operator GUID() const {return m_guid;}

		bool operator ==(const CGuid &gid) const { return memcmp((const void*)&m_guid, (const void*)&gid.m_guid, sizeof(m_guid)) == 0; }
		bool operator !=(const CGuid &gid) const { return memcmp((const void*)&m_guid, (const void*)&gid.m_guid, sizeof(m_guid)) != 0; }
		bool operator <(const CGuid &gid) const  { return memcmp((const void*)&m_guid, (const void*)&gid.m_guid, sizeof(m_guid)) < 0; }
		bool operator >(const CGuid &gid) const  { return memcmp((const void*)&m_guid, (const void*)&gid.m_guid, sizeof(m_guid)) > 0; }
		bool operator <=(const CGuid &gid) const { return memcmp((const void*)&m_guid, (const void*)&gid.m_guid, sizeof(m_guid)) <= 0; }
		bool operator >=(const CGuid &gid) const { return memcmp((const void*)&m_guid, (const void*)&gid.m_guid, sizeof(m_guid)) >= 0; }

		void setNull() { m_guid = guid_null; }
		void generate() { ::CoCreateGuid(&m_guid); }

	protected:
		GUID m_guid;
	};
}

namespace stdext
{
	inline size_t hash_value(const CommonLib::CGuid& _Guid)
	{
		//	  typedef size_t _Strsize;

		size_t _Val = _HASH_SEED;
		size_t _Size = sizeof(GUID) / sizeof (size_t);
		GUID __Guid = (GUID)_Guid;

		const size_t *_Ptr = (const size_t *)&__Guid;
		for (size_t _Idx = 0; _Idx < _Size; _Idx ++)
			_Val += _Ptr[_Idx];

		return _Val;
	}
}

#else //not _WIN32
#include "CString.h"

namespace CommonLib
{
	class CGuid 
	{
	public:
		CGuid(void) {};
		CGuid(const CGuid &gid) {};

		~CGuid(void) {};

		CString toString() const { return CString("0"); }

		CGuid& operator =(const CGuid& gid) {return *this; }

		bool operator ==(const CGuid &gid) const { return 0;};
		bool operator !=(const CGuid &gid) const { return 0;};
		bool operator <(const CGuid &gid) const  { return 0;};
		bool operator >(const CGuid &gid) const  { return 0;};
		bool operator <=(const CGuid &gid) const { return 0;};
		bool operator >=(const CGuid &gid) const { return 0;};
	};
}

#endif //_WIN32