#pragma once





class ACComp
{
public:

	ACComp();
	~ACComp();


	void compressInteger(const char *pszText, CommonLib::IWriteStream* pStream);

	void compress(const char *pszText, CommonLib::IWriteStream* pStream);
	void decompress(CommonLib::IReadStream* pStream, CommonLib::CString& str);
private:





};