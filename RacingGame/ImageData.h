#ifndef IMAGE_DATA_H
#define IMAGE_DATA_H

#include "Int.h"
#include "Collections.h"
#include "BinIO.h"

class ImageData {
public:
	ImageData() = default;
	ImageData(u32 width, u32 height);
	~ImageData() = default;

	ImageData& from(const String& fileName);
	ImageData& from(u8* data);

	void set(u32 x, u32 y, u8 r, u8 g, u8 b, u8 a = 0xFF);

	u32 width() const { return m_width; }
	u32 height() const { return m_height; }

	u8* pixels() { return m_pixels.data(); }

private:
	Vec<u8> m_pixels;
	u32 m_width, m_height;

	void loadUncompressed(BinReader* rd);
	void loadCompressed(BinReader* rd);
};

#endif // IMAGE_DATA_H