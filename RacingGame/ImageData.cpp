#include "ImageData.h"

#include "Logger.h"
#include <fstream>

ImageData::ImageData(u32 width, u32 height) {
	m_width = width;
	m_height = height;
	m_pixels.resize(width * height * 4);
}

ImageData& ImageData::from(const String& fileName) {
	std::ifstream fs(fileName, std::ios::ate | std::ios::binary);
	if (fs.good()) {
		auto pos = fs.tellg();
		u8 *data = new u8[pos];
		fs.seekg(0, std::ios::beg);
		fs.read(reinterpret_cast<char*>(data), pos);
		fs.close();

		from(data);

		delete[] data;
	}
	return *this;
}

ImageData& ImageData::from(u8* data) {
	// Uncompressed TGA Header
	const u8 uTGAcompare[8] = { 0,0, 2,0,0,0,0,0 };
	// Compressed TGA Header
	const u8 cTGAcompare[8] = { 0,0,10,0,0,0,0,0 };

	UPtr<BinReader> rd(new BinReader(data));

	struct TGAHeader {
		u8 data[8];
	} header = rd->read<TGAHeader>();

	if (std::memcmp(header.data, cTGAcompare, 8) == 0) {
		loadCompressed(rd.get());
	} else if (std::memcmp(header.data, cTGAcompare, 8) == 0) {
		loadUncompressed(rd.get());
	} else {
		LogError("Load Failed: Invalid TGA image.");
	}

	return *this;
}

void ImageData::set(u32 x, u32 y, u8 r, u8 g, u8 b, u8 a) {
	u32 index = (x + y * m_width) * 4;
	m_pixels[index + 0] = r;
	m_pixels[index + 1] = g;
	m_pixels[index + 2] = b;
	m_pixels[index + 3] = a;
}

void ImageData::loadUncompressed(BinReader* rd) {
	rd->read<u16>(); // X origin
	rd->read<u16>(); // Y origin

	m_width = u32(rd->read<u16>());
	m_height = u32(rd->read<u16>());
	u8 bpp = rd->read();
	u8 desc = rd->read();
	u8 comps = bpp / 8;

	if ((bpp != 24 && bpp != 32) || m_width == 0 || m_height == 0) {
		LogError("Load Failed: Invalid image dimensions/BPP");
		return;
	}

	m_pixels.resize(m_width * m_height * 4);

	for (u32 i = 0; i < m_width * m_height; i++) {
		u32 j = i * 4;
		m_pixels[j + 2] = rd->read();
		m_pixels[j + 1] = rd->read();
		m_pixels[j + 0] = rd->read();
		if (comps == 4) {
			m_pixels[j + 3] = rd->read();
		} else {
			m_pixels[j + 3] = 0;
		}
	}
}

void ImageData::loadCompressed(BinReader* rd) {
	rd->read<u16>(); // X origin
	rd->read<u16>(); // Y origin

	m_width = u32(rd->read<u16>());
	m_height = u32(rd->read<u16>());
	u8 bpp = rd->read();
	u8 desc = rd->read();
	u8 comps = bpp / 8;

	if ((bpp != 24 && bpp != 32) || m_width == 0 || m_height == 0) {
		LogError("Load Failed: Invalid image dimensions/BPP");
		return;
	}

	m_pixels.resize(m_width * m_height * 4);

	u32 currentByte = 0;
	u32 currentPixel = 0;
	do {
		u8 chunkHeader = rd->read();
		if (chunkHeader < 128) { // RAW
			chunkHeader++;
			for (u8 i = 0; i < chunkHeader; i++) {
				m_pixels[currentByte + 2] = rd->read();
				m_pixels[currentByte + 1] = rd->read();
				m_pixels[currentByte + 0] = rd->read();
				if (comps == 4) {
					m_pixels[currentByte + 3] = rd->read();
				} else {
					m_pixels[currentByte + 3] = 0;
				}
				currentByte += 4;
				currentPixel++;
			}
		} else { // RLE
			chunkHeader -= 127;
			u8 r = rd->read();
			u8 g = rd->read();
			u8 b = rd->read();
			u8 a = comps == 4 ? rd->read() : 255;
			for (u8 i = 0; i < chunkHeader; i++) {
				m_pixels[currentByte + 3] = a;
				m_pixels[currentByte + 2] = r;
				m_pixels[currentByte + 1] = g;
				m_pixels[currentByte + 0] = b;
				currentByte += 4;
				currentPixel++;
			}
		}
	} while (currentPixel < m_width * m_height);
}
