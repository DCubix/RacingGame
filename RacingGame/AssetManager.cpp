#include "AssetManager.h"

#include "Logger.h"
#include <cctype>

AssetManager::~AssetManager() {
	mz_zip_reader_end(&m_zipFile);
}

void AssetManager::init(const String& zipFile) {
	std::memset(&m_zipFile, 0, sizeof(ZIPFile));
	auto status = mz_zip_reader_init_file(&m_zipFile, zipFile.c_str(), 0);
	if (!status) {
		LogError(mz_zip_get_error_string(mz_zip_get_last_error(&m_zipFile)));
		return;
	}

	u32 numFiles = mz_zip_reader_get_num_files(&m_zipFile);
	for (u32 i = 0; i < numFiles; i++) {
		mz_zip_archive_file_stat stat;
		mz_zip_reader_file_stat(&m_zipFile, i, &stat);
		
		const String fileName(stat.m_filename);
		String ext = fileName.substr(fileName.find_last_of('.')+1);
		std::transform(ext.begin(), ext.end(), ext.begin(), std::tolower);

		if (ext == "tga") {
			addImage(fileName);
		}
	}
}

void AssetManager::load() {
	u32 numFiles = u32(mz_zip_reader_get_num_files(&m_zipFile));
	if (numFiles == 0) {
		LogWarning("No files in archive.");
		return;
	}
	for (auto&& asset : m_assetQueue) {
		int loc = getFile(asset.fileName);
		if (loc == -1) {
			LogWarning("File not found: \"", asset.fileName, "\"");
			continue;
		}

		switch (asset.type) {
			case AssetType::Image:
			{
				size_t fileSize;
				void* fileData = mz_zip_reader_extract_to_heap(&m_zipFile, loc, &fileSize, 0);
				if (!fileData) {
					LogError("Failed to extract: \"", asset.fileName, "\"");
				}

				UPtr<ImageData> img(new ImageData());
				img->from(reinterpret_cast<u8*>(fileData));
				
				m_images.insert({ asset.fileName, std::move(img) });

				mz_free(fileData);
				LogInfo("Loaded: \"", asset.fileName, "\"");
			} break;
			default: break;
		}
	}
}

void AssetManager::addImage(const String& fileName) {
	m_assetQueue.push_back({ AssetType::Image, fileName });
}

Texture2D AssetManager::getTexture(const String& imageFile) {
	if (m_textures.find(imageFile) == m_textures.end()) {
		Texture2D tex = Texture2DFactory::create()
			.setFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR)
			.setWrap(GL_REPEAT, GL_REPEAT)
			.setData(*getImage(imageFile))
			.generateMipmaps();
		m_textures.insert({ imageFile, tex });
	}
	return m_textures[imageFile];
}

int AssetManager::getFile(const String& fileName) {
	return mz_zip_reader_locate_file(&m_zipFile, fileName.c_str(), nullptr, MZ_ZIP_FLAG_CASE_SENSITIVE);
}
