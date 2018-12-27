#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include "glad.h"
#include "Memory.h"
#include "Int.h"
#include "Collections.h"
#include "ShaderProgram.h"

#include "Box2D/Box2D.h"

class DebugDraw {
public:
	~DebugDraw();

	void init();
	void flush(const glm::mat4& vp);

	void line(const glm::vec3& from, const glm::vec3& to, const glm::vec4& color);
	void dot(const glm::vec3& pos, const glm::vec4& color, float size = 0.1f);
	void circle(const glm::vec3& pos, float radius, const glm::vec4& color);

	static DebugDraw& get();

private:
	DebugDraw() = default;

	ShaderProgram m_shader;
	GLuint m_vbo, m_vao;
	u32 m_vboSize;
	Vec<float> m_vertices;

	static UPtr<DebugDraw> s_instance;
};

class PhysicsDebugDraw : public b2Draw {
public:
	PhysicsDebugDraw() : b2Draw(), ppm(1.0f) {}

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	void DrawTransform(const b2Transform& xf);

	float ppm;
};

#endif // DEBUG_DRAW_H