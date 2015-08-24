#ifndef GIS_ENGINE_DISPLAY_BITMAP_H_
#define GIS_ENGINE_DISPLAY_BITMAP_H_
#include "GraphTypes.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "Color.h"
#include "CommonLibrary/IRefCnt.h"
namespace GisEngine
{
	namespace Display
	{

		class CBitmap : public CommonLib::AutoRefCounter
		{
		public:
			CBitmap(CommonLib::alloc_t *pAlloc = NULL);
			CBitmap(CommonLib::IReadStream *pStream, CommonLib::alloc_t *pAlloc = NULL);
			CBitmap(size_t width, size_t height, eBitmapFormatType type, CommonLib::alloc_t *pAlloc = NULL);
			CBitmap(unsigned char* bits, size_t width, size_t height, 
				eBitmapFormatType type, Color* palette = 0, bool release = false, CommonLib::alloc_t *pAlloc = NULL);
			CBitmap(const CBitmap& bmp);
			~CBitmap(void);
			CBitmap& operator=(const CBitmap& bmp);

			bool save(CommonLib::IWriteStream *pStream) const;
			bool load(CommonLib::IReadStream *pStream);

			bool saveXML(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszNodeName = L"bitmap") const;
			bool load(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszNodeName = L"bitmap");

		public:
			size_t                 height() const;
			size_t                 width() const;
			unsigned char*         bits();
			const unsigned char*   bits() const;
			size_t                 lineSize() const;
			size_t                 size() const;
			size_t                 bpp() const;
			Color*                 palette();
			eBitmapFormatType       type() const;
			Color                  pixel(size_t row, size_t col);
			bool                   copy_to(CBitmap* bitmap);
			void                   attach(unsigned char* bits, size_t width, size_t height, eBitmapFormatType type, Color* palette = 0, bool release = false);
			CBitmap*                transform(double xScale, double yScale, double angle = 0);
			bool                   importFromPNG(const unsigned char* data, size_t len);
			bool                   importFromJPG(const unsigned char* data, size_t len);
			void init(size_t width, size_t height, eBitmapFormatType type);
		private:
			unsigned char*   m_pBuf;
			size_t           m_nWidth;
			size_t           m_nHeight;
			eBitmapFormatType m_type;
			Color*           m_pPalette;
			bool             m_bRelease;
			CommonLib::alloc_t*		m_pAlloc;
			CommonLib::simple_alloc_t m_alloc;
		};

		typedef CommonLib::IRefCntPtr<CBitmap> BitmapPtr;
	}
}

#endif