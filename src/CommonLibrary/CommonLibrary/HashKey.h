#ifndef _LIB_COMMON_HASH_KEY_H_
#define _LIB_COMMON_HASH_KEY_H_
#include "general.h"
#include "String.h"

namespace CommonLib
{
	template <class T> size_t GetTypedSize(const T &t)
	{
		return sizeof(t);
	}

	static size_t GetTypedSize(const char *pszStr)
	{
		if(pszStr == NULL)
			return sizeof(pszStr);

		return strlen(pszStr) + sizeof(pszStr);
	}
	static size_t GetTypedSize(const wchar_t *pszStr)
	{
		if(pszStr == NULL)
			return sizeof(pszStr);

		return wcslen(pszStr) + sizeof(pszStr);
	}

	static size_t GetTypedSize(const CommonLib::CString &str)
	{			
		return str.length() + sizeof(str);
	}

 
 

	template<class T> class CHashKey
	{
	protected:
		T m_t;
	public:
		explicit CHashKey(const T& t) : m_t(t) {}
		bool operator<(const CHashKey<T>& key) const { return m_t < key.m_t; }
		bool operator==(const CHashKey<T>& key) const { return m_t == key.m_t; }

		size_t size() const
		{
			return GetTypedSize(m_t);
		}
	};

	template<class T1, class T2> class CHash2Key
	{
	protected:
		T1 m_t1;
		T2 m_t2;
	public:
		explicit CHash2Key(const T1& t1, const T2& t2) : m_t1(t1), m_t2(t2) {}
		bool operator<(const CHash2Key<T1, T2>& key) const {
			if (m_t1 < key.m_t1)
				return true;
			else if (key.m_t1 < m_t1)
				return false;
			return m_t2 < key.m_t2;
		}
		bool operator==(const CHash2Key<T1, T2>& key) const { return m_t1 == key.m_t1 && m_t2 == key.m_t2; }

		const T1& GetT1()const { return m_t1; }
		const T2& GetT2()const { return m_t2; }

		size_t size() const
		{
			return GetTypedSize(m_t1) + GetTypedSize(m_t2);
		}
	};

	template<class T1, class T2, class T3> class CHash3Key
	{
	protected:
		T1 m_t1;
		T2 m_t2;
		T3 m_t3;
	public:
		explicit CHash3Key(const T1& t1, const T2& t2, const T3& t3) : m_t1(t1), m_t2(t2), m_t3(t3) {}
		bool operator<(const CHash3Key<T1, T2, T3>& key) const {
			if (m_t1 < key.m_t1)
				return true;
			else if (key.m_t1 < m_t1)
				return false;
			if (m_t2 < key.m_t2)
				return true;
			else if (key.m_t2 < m_t2)
				return false;
			return m_t3 < key.m_t3;
		}
		bool operator==(const CHash3Key<T1, T2, T3>& key) const { return m_t1 == key.m_t1 && m_t2 == key.m_t2 && m_t3 == key.m_t3; }

		const T1& GetT1()const { return m_t1; }
		const T2& GetT2()const { return m_t2; }
		const T3& GetT3()const { return m_t3; }

		size_t size() const
		{
			return GetTypedSize(m_t1) + GetTypedSize(m_t2) + GetTypedSize(m_t3);
		}
	};

	template<class T1, class T2, class T3, class T4> class CHash4Key
	{
	protected:
		T1 m_t1;
		T2 m_t2;
		T3 m_t3;
		T4 m_t4;
	public:
		explicit CHash4Key(const T1& t1, const T2& t2, const T3& t3, const T4& t4) : m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4) {}
		bool operator<(const CHash4Key<T1, T2, T3, T4>& key) const {
			if (m_t1 < key.m_t1)
				return true;
			else if (key.m_t1 < m_t1)
				return false;
			if (m_t2 < key.m_t2)
				return true;
			else if (key.m_t2 < m_t2)
				return false;
			if (m_t3 < key.m_t3)
				return true;
			else if (key.m_t3 < m_t3)
				return false;
			return m_t4 < key.m_t4;
		}
		bool operator==(const CHash4Key<T1, T2, T3, T4>& key) const {
			return m_t1 == key.m_t1 &&
				m_t2 == key.m_t2 &&
				m_t3 == key.m_t3 &&
				m_t4 == key.m_t4;
		}

		const T1& GetT1()const { return m_t1; }
		const T2& GetT2()const { return m_t2; }
		const T3& GetT3()const { return m_t3; }
		const T4& GetT4()const { return m_t4; }

		size_t size() const
		{
			return GetTypedSize(m_t1) + GetTypedSize(m_t2) + GetTypedSize(m_t3) + GetTypedSize(m_t4);
		}
	};

	template<class T1, class T2, class T3, class T4, class T5> class CHash5Key
	{
	protected:
		T1 m_t1;
		T2 m_t2;
		T3 m_t3;
		T4 m_t4;
		T5 m_t5;
	public:
		explicit CHash5Key(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
			: m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4), m_t5(t5) {}
		bool operator<(const CHash5Key<T1, T2, T3, T4, T5>& key) const {
			if (m_t1 < key.m_t1)
				return true;
			else if (key.m_t1 < m_t1)
				return false;
			if (m_t2 < key.m_t2)
				return true;
			else if (key.m_t2 < m_t2)
				return false;
			if (m_t3 < key.m_t3)
				return true;
			else if (key.m_t3 < m_t3)
				return false;
			if (m_t4 < key.m_t4)
				return true;
			else if (key.m_t4 < m_t4)
				return false;
			return m_t5 < key.m_t5;
		}
		bool operator==(const CHash5Key<T1, T2, T3, T4, T5>& key) const {
			return m_t1 == key.m_t1 &&
				m_t2 == key.m_t2 &&
				m_t3 == key.m_t3 &&
				m_t4 == key.m_t4 &&
				m_t5 == key.m_t5;
		}

		const T1& GetT1()const { return m_t1; }
		const T2& GetT2()const { return m_t2; }
		const T3& GetT3()const { return m_t3; }
		const T4& GetT4()const { return m_t4; }
		const T5& GetT5()const { return m_t5; }

		size_t size() const
		{
			return GetTypedSize(m_t1) + GetTypedSize(m_t2) +
				GetTypedSize(m_t3) + GetTypedSize(m_t4) + GetTypedSize(m_t5);
		}
	};
}


#endif