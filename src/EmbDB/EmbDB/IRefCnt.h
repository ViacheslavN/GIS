#ifndef _EMBEDDED_DATABASE_I_REF_CNT_H_
#define _EMBEDDED_DATABASE_I_REF_CNT_H_

#include "CommonLibrary/Interlocked.h"
#include "CommonLibrary/delegate.h"
namespace embDB
{
	class IRefCnt {
	public:
		virtual int AddRef() const =0;
		virtual int Release() =0;
	protected:
		virtual ~IRefCnt() {}
	};

	class RefCounter;
	typedef CommonLib::delegate1_t<RefCounter*> TRemoveFunk;
	class RefCounter : public IRefCnt
	{
	public:
		RefCounter(TRemoveFunk *pRem = NULL) : m_nCounter(0), m_pRemFunk(pRem)
		{}
		RefCounter(LONG val, TRemoveFunk *pRem = NULL) : m_nCounter(val) {}
		~RefCounter(){};
		virtual int AddRef() const { return CommonLib::Interlocked::Increment(&m_nCounter);}
		virtual int Release()  {
			if(CommonLib::Interlocked::Decrement(&m_nCounter) != 0)
			{
				return m_nCounter;
			}
			if(m_pRemFunk)
			{
				m_pRemFunk->operator()(this);
			}
			return 0;
		}
		bool isRemovable() const { return 0 == m_nCounter; }
		TRemoveFunk* m_pRemFunk;
	private:
		mutable CommonLib::Interlocked::inc_type m_nCounter;
	
		
	};



	class AutoRefCounter : public IRefCnt
	{
	public:
		AutoRefCounter() : m_nCounter(0)
		{}
		AutoRefCounter(LONG val) : m_nCounter(val) {}
		~AutoRefCounter(){};
		virtual int AddRef() const { return CommonLib::Interlocked::Increment(&m_nCounter);}
		virtual int Release()  {
			if(CommonLib::Interlocked::Decrement(&m_nCounter) != 0)
			{
				return m_nCounter;
			}
			delete this;
			return 0;
		}
		bool isRemovable() const { return 0 == m_nCounter; }
	private:
		mutable CommonLib::Interlocked::inc_type m_nCounter;


	};


	template<class Obj>
	class IRefCntPtr
	{
	public:
		explicit IRefCntPtr(Obj* pObj = 0, bool addRef = true)
			: m_pObj(pObj)
		{
			if (m_pObj != NULL && addRef)
				m_pObj->AddRef();
		}
		IRefCntPtr(const IRefCntPtr& refCntPtr)
			: m_pObj(refCntPtr.m_pObj)
		{
			if (m_pObj != 0)
				m_pObj->AddRef();
		}

		IRefCntPtr& operator=(const IRefCntPtr& refCntPtr)
		{
			if (refCntPtr.m_pObj != 0)
				refCntPtr.m_pObj->AddRef();
			if (m_pObj != 0)
				m_pObj->Release();
			m_pObj = refCntPtr.m_pObj;
			return *this;
		}
		IRefCntPtr& operator=(Obj* pObj)
		{
			if (pObj != 0)
				pObj->AddRef();
			if (m_pObj != 0)
				m_pObj->Release();
			m_pObj = pObj;
			return *this;
		}

		bool operator != (const IRefCntPtr& refCntPtr)
		{
			return m_pObj != refCntPtr.m_pObj;
		}
		bool operator == (const IRefCntPtr& refCntPtr)
		{
			return m_pObj == refCntPtr.m_pObj;
		}



		bool operator != (const Obj* pObj)
		{
			return m_pObj != pObj;
		}
		bool operator == (const Obj* pObj)
		{
			return m_pObj == pObj;
		}


		~IRefCntPtr()
		{
			if (m_pObj != 0)
				m_pObj->Release();
		}

		Obj* get() const { return m_pObj; }
		Obj* operator->() const { return m_pObj; }
		Obj& operator*() const { return *m_pObj; }
		Obj* release() 
		{ 
			if (m_pObj != 0)
				m_pObj->Release();
			Obj* pTmp = m_pObj; 
			m_pObj = 0;
			return pTmp;
		}
		void reset(Obj* m_pObj = 0, bool addRef = true)
		{
			if (m_pObj != 0)
				m_pObj->Release();
			m_pObj = m_pObj;
			if (m_pObj != NULL && addRef)
				m_pObj->AddRef();
		}

	private:
		Obj* m_pObj;
	};
	 
}
#endif


