#include "Texture.h"

Vec<Texture2D> Factory<Texture2D>::s_textures;

Texture2D& Texture2D::setData(ImageData& data) {
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA8,
		data.width(), data.height(),
		0, GL_RGBA, GL_UNSIGNED_BYTE,
		data.pixels()
	);
	m_width = data.width();
	m_height  = data.height();
	return *this;
}

Texture2D& Texture2D::setFilter(GLenum min, GLenum mag) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
	return *this;
}

Texture2D& Texture2D::setWrap(GLenum s, GLenum t) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
	return *this;
}

Texture2D& Texture2D::generateMipmaps() {
	glGenerateMipmap(GL_TEXTURE_2D);
	return *this;
}

void Texture2D::bind(u32 slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture2D::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}
