#ifndef _EMBEDDED_DATABASE_GLOBAL_PARAMS_H_
#define _EMBEDDED_DATABASE_GLOBAL_PARAMS_H_


namespace embDB
{

	class CGlobalParams
	{
		public:
			static CGlobalParams& Instance();
			void SetCheckCRC(bool bCheck);
			bool GetCheckCRC() const;

		private:
			CGlobalParams();
			~CGlobalParams();
		private:
			bool m_bCheckCRC;

	};
}

#endif