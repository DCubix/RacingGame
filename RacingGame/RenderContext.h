#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

// Mesh Batching Renderer

#include "glm/mat4x4.hpp"
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Texture.h"

#include "glad.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#define MAX_LIGHTS 32

struct Light {
	enum LightType {
		Disabled = 0,
		Sun,
		Point,
		Spot
	};

	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 color;
	float intensity;
	float radius;
	float spotCutoff;
	LightType type;
};

struct Drawable {
	bool transform;
	glm::mat4 modelMatrix;
	Vec<Vertex> vertices;
	Vec<u32> indices;
	Texture2D color, normal, specular;
};

struct Batch {
	bool transform;
	glm::mat4 modelMatrix;
	u32 offset, length;
	Texture2D color, normal, specular;

	Batch() = default;
	Batch(const glm::mat4& m, u32 off, u32 len, Texture2D col, Texture2D nrm, Texture2D spc, bool xform)
		: modelMatrix(m), offset(off), length(len), color(col), normal(nrm), specular(spc), transform(xform)
	{}
};

struct Cursor {
	friend class RenderContext;

	Cursor& position(const glm::vec3& v) { m_position = v; return *this; }
	Cursor& scale(const glm::vec2& v) { m_scale = v; return *this; }
	Cursor& origin(const glm::vec3& v) { m_origin = v; return *this; }
	Cursor& rotation(float v) { m_rotation = v; return *this; }
	Cursor& region(const glm::vec4& v) { m_region = v; return *this; }

protected:
	glm::vec4 m_region{ 0, 0, 1, 1 };
	glm::vec3 m_position{ 0, 0, 0 }, m_origin{ 0.5f, 0.5f, 0.5f };
	glm::vec2 m_scale{ 1, 1 };
	float m_rotation{ 0 };
};

class RenderContext {
public:
	RenderContext();
	~RenderContext();

	void begin();
	void end();

	void submit(
		const Mesh& mesh,
		const glm::mat4& modelMatrix,
		Texture2D color,
		Texture2D normal,
		Texture2D specular
	);

	void submitSprite(
		Texture2D color,
		Texture2D normal,
		Texture2D specular,
		glm::vec4 vcolor = glm::vec4(1)
	);

	void submitSunLight(const glm::vec3& direction, const glm::vec3& color, float intensity);
	void submitPointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float radius);
	void submitSpotLight(
		const glm::vec3& position,
		const glm::vec3& direction,
		const glm::vec3& color,
		float intensity,
		float radius,
		float cutOff
	);

	glm::mat4 projection() const { return m_projection; }
	void projection(const glm::mat4& vp) { m_projection = vp;  }

	glm::mat4 view() const { return m_view; }
	void view(const glm::mat4& vp) { m_view = vp; }

	glm::vec3 ambient() const { return m_ambient; }
	void ambient(const glm::vec3& a) { m_ambient = a; }

	Texture2D environment() const { m_environment; }
	void environment(const Texture2D& tex) { m_environment = tex;  }

	Cursor& cursor() { return m_cursor; }

private:
	Vec<Drawable> m_drawables;
	Vec<Batch> m_batches;

	Array<Light, MAX_LIGHTS> m_lights;
	u32 m_lightCount;
	glm::vec3 m_ambient;

	GLuint m_vbo, m_ebo, m_vao;
	u32 m_vboSize, m_eboSize;

	ShaderProgram m_shader;
	Texture2D m_environment;
	Cursor m_cursor;

	glm::mat4 m_projection, m_view;

	void updateBufferData();
};

#endif // RENDER_CONTEXT_H