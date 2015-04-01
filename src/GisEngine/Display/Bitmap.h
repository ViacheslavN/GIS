#ifndef GIS_ENGINE_DISPLAY_BITMAP_H_
#define GIS_ENGINE_DISPLAY_BITMAP_H_
#include "GraphTypes.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "Color.h"
namespace GisEngine
{
	namespace Display
	{

		class CBitmap
		{
		public:
			CBitmap();
			CBitmap(CommonLib::IReadStream *pStream);
			CBitmap(size_t width, size_t height, BitmapFormatType type);
			CBitmap(unsigned char* bits, size_t width, size_t height, BitmapFormatType type, Color* palette = 0, bool release = false);
			CBitmap(const CBitmap& bmp);
			~CBitmap(void);
			CBitmap& operator=(const CBitmap& bmp);

			void save(CommonLib::IWriteStream *pStream) const;
			void load(CommonLib::IReadStream *pStream);
		public:
			size_t                 height() const;
			size_t                 width() const;
			unsigned char*         bits();
			const unsigned char*   bits() const;
			size_t                 lineSize() const;
			size_t                 size() const;
			size_t                 bpp() const;
			Color*                 palette();
			BitmapFormatType       type() const;
			Color                  pixel(size_t row, size_t col);
			bool                   copy_to(CBitmap* bitmap);
			void                   attach(unsigned char* bits, size_t width, size_t height, BitmapFormatType type, Color* palette = 0, bool release = false);
			CBitmap*                transform(double xScale, double yScale, double angle = 0);
			bool                   importFromPNG(const unsigned char* data, size_t len);
			bool                   importFromJPG(const unsigned char* data, size_t len);
			void init(size_t width, size_t height, BitmapFormatType type);
		private:
			unsigned char*   buf_;
			size_t           width_;
			size_t           height_;
			BitmapFormatType type_;
			Color*           palette_;
			bool             release_;
		};
	}
}

#endif