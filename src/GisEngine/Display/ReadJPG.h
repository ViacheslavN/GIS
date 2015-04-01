#ifndef GIS_ENGINE_DISPLAY_JPG_READER_H_
#define GIS_ENGINE_DISPLAY_JPG_READER_H_
#include "GraphTypes.h"
#include "Common/Common.h"
#include "CommonLibrary/GeneralTypes.h"


namespace GisEngine
{
	namespace Display
	{

		class CBitmap;
		class ReadJPG
		{
		public:
			ReadJPG(const BYTE* ptr = 0, int length = 0);

			CBitmap*  read();
			bool read(CBitmap* pBitmap);

		private:

			bool  readJPG(CBitmap* pBitmap);
			int length_;
			const BYTE * data_;

		};
	}
}

#endif