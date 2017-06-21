#ifndef _EMBEDDED_USER_CRYPTO_MANAGER_H_
#define _EMBEDDED_USER_CRYPTO_MANAGER_H_

 #include "../Crypto/PageCipher.h"
namespace embDB
{
	class CDatabase;



	class CUserCryptoManager
	{
		public:


			enum eUserMode
			{
				eNoneMode,
				ePasswordMode,
				eMultiUser
			};

			CUserCryptoManager(CDatabase *pDB);
			~CUserCryptoManager();


			static const uint32 ___nSALT_SIZE = 16; 

			bool init(int64 nAddr, const wchar_t *pszPassword, QryptoALG qryptoAlg);
			bool init(int64 nAddr, const wchar_t *pszAdmName, const wchar_t *pszPassword, QryptoALG qryptoAlg);

			bool addUser(const wchar_t *pszName, const wchar_t *pszPassword, eUserGroup userGroup, IDBTransaction *pTran);
			bool deleteUser(const wchar_t *pszName, const wchar_t *pszPassword, eUserGroup userGroup, IDBTransaction *pTran);



			bool load(int64 nAddr);
			void close();


			uint64 GetUserID(const wchar_t *pszUser);
			eUserGroup GetUserGroup(const wchar_t *pszUser);


			eUserMode getMode() const;


			bool CheckUser(const wchar_t *pszUser, const wchar_t *pszPassword);
			bool GetKey(const wchar_t *pszUser, const wchar_t *pszPassword, CommonLib::CBlob& blob);
			int64 GetOffset() const;
			CPageCipher * GetPageCipher() {return m_PageChiper.get();}
	private:
			bool InitPasswordMode(CFilePage *pPage, const wchar_t *pszPassword);
			uint32 CalcOffset(const byte* pPWD, uint32 nLen, const byte* pSlat, uint32 nSaltLen) const;
			void CreateNoise(uint32 nSize);
			int64 CreateCheckPWDPage();
			bool LoadPasswordMode(CommonLib::FxMemoryReadStream *pStream);
			bool CheckPassword( const wchar_t *pszPassword);

	private:
		CDatabase *m_pDB;
		eUserMode m_UserMode;
		QryptoALG m_qryptoAlg;
		uint32 m_nOffset;

		std::auto_ptr<CPageCipher> m_PageChiper;

		//for password mode
		int64 m_nPWDPage;
		bool m_bPWDCheck;
		byte m_szSalt[___nSALT_SIZE];
		byte m_szSaltIV[___nSALT_SIZE];
		std::auto_ptr<CPageCipher> m_pCheckPageChiper;

		//for muitl user
		int64 m_nUsersList;
	
 
	

	

	};
}

#endif