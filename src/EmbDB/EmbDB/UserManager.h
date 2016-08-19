#ifndef _EMBEDDED_USER_MANAGER_H_
#define _EMBEDDED_USER_MANAGER_H_

 
namespace embDB
{
	class CDatabase;


	enum eUserMode
	{
		eNoneMode,
		eNoUserMode,
		eMultiUser
	};

	class CUserManager
	{
		public:
			CUserManager(CDatabase *pDB);
			~CUserManager();

			bool init(int64 nAddr, eUserMode mode);
			bool load(int64 nAddr);

			eUserMode getMode() const;


			bool CheckUser(const wchar_t *pszUser, const wchar_t *pszPassword);
			bool GetKey(const wchar_t *pszUser, const wchar_t *pszPassword, CommonLib::CBlob& blob);
	private:
		CDatabase *m_pDB;
		eUserMode m_UserMode;
	};
}

#endif