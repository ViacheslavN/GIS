#ifndef _EMBEDDED_DATABASE_AES_256_H_
#define _EMBEDDED_DATABASE_AES_256_H_


namespace embDB
{


	namespace Crypto
	{
		class CAES256 : public CBaseAES
		{
		public:
			CAES256(){}
			~CAES256(){}

			virtual uint32 getKeyLength() const {return 32;}
		protected:
			virtual uint32 getNK() const {return 8;}
			virtual uint32 getNR() const {return 14;}
		};
	}
}

#endif