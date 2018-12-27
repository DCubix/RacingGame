#ifndef TEXTURE_H
#define TEXTURE_H

#include "glad.h"
#include "ImageData.h"
#include "Factory.h"

class Texture2D {
	friend class Factory<Texture2D>;
public:
	Texture2D() : m_id(0), m_width(0), m_height(0) {}

	Texture2D& setData(ImageData& data);
	Texture2D& setFilter(GLenum min, GLenum mag);
	Texture2D& setWrap(GLenum s, GLenum t);
	Texture2D& generateMipmaps();

	void bind(u32 slot = 0);
	void unbind();

	u32 width() const { return m_width; }
	u32 height() const { return m_height; }
	GLuint id() const { return m_id; }

private:
	u32 m_width, m_height;
	GLuint m_id;
};

template<>
class Factory<Texture2D> {
public:
	static Texture2D& create() {
		Texture2D tex{};
		glGenTextures(1, &tex.m_id);
		glBindTexture(GL_TEXTURE_2D, tex.m_id);
		s_textures.push_back(tex);
		return s_textures.back();
	}

	static void release() {
		for (auto&& tex : s_textures) {
			glDeleteTextures(1, &tex.m_id);
		}
		s_textures.clear();
	}
private:
	static Vec<Texture2D> s_textures;
};

using Texture2DFactory = Factory<Texture2D>;

#endif // TEXTURE_H