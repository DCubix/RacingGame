#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "ImageData.h"
#include "Texture.h"
#include "Collections.h"

#include "miniz.h"

using ZIPFile = mz_zip_archive;

enum AssetType {
	Image = 0
};

struct Asset {
	AssetType type;
	String fileName;
};

class AssetManager {
public:
	AssetManager() = default;
	~AssetManager();

	void init(const String& zipFile);
	void load();

	void addImage(const String& fileName);
	ImageData* getImage(const String& fileName) { return m_images[fileName].get(); }

	Texture2D getTexture(const String& imageFile);

private:
	int getFile(const String& fileName);

	Vec<Asset> m_assetQueue;
	UMap<String, UPtr<ImageData>> m_images;

	UMap<String, Texture2D> m_textures;

	ZIPFile m_zipFile;
};

#endif // ASSET_MANAGER_H