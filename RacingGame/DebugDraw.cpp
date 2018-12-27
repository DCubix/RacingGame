#include "DebugDraw.h"

UPtr<DebugDraw> DebugDraw::s_instance;

DebugDraw::~DebugDraw() {
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}

void DebugDraw::init() {
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 28, (void*) 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, 28, (void*) 12);

	glBindVertexArray(0);

	const String VS = R"(#version 330 core
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec4 vColor;

uniform mat4 uViewProj;

out vec4 oColor;

void main() {
	gl_Position = uViewProj * vec4(vPosition, 1.0);
	oColor = vColor;
})";
	const String FS = R"(#version 330 core
out vec4 fragColor;

in vec4 oColor;
void main() {
	fragColor = oColor;
})";

	m_shader = ShaderFactory::create()
		.addSource(ShaderProgram::VertexShader, VS)
		.addSource(ShaderProgram::FragmentShader, FS)
		.link();
}

void DebugDraw::flush(const glm::mat4& vp) {
	bool depthEnabled = glIsEnabled(GL_DEPTH_TEST);
	if (depthEnabled) {
		glDisable(GL_DEPTH_TEST);
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	if (m_vertices.size() > m_vboSize) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertices.size(), m_vertices.data(), GL_DYNAMIC_DRAW);
		m_vboSize = m_vertices.size();
	} else {
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_vertices.size(), m_vertices.data());
	}

	m_shader.use();
	m_shader.get("uViewProj").set(vp);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_LINES, 0, m_vertices.size() / 7);
	glBindVertexArray(0);

	m_vertices.clear();

	if (depthEnabled) {
		glEnable(GL_DEPTH_TEST);
	}
}

void DebugDraw::line(const glm::vec3& from, const glm::vec3& to, const glm::vec4& color) {
	m_vertices.push_back(from.x);
	m_vertices.push_back(from.y);
	m_vertices.push_back(from.z);
	m_vertices.push_back(color.x);
	m_vertices.push_back(color.y);
	m_vertices.push_back(color.z);
	m_vertices.push_back(color.w);
	m_vertices.push_back(to.x);
	m_vertices.push_back(to.y);
	m_vertices.push_back(to.z);
	m_vertices.push_back(color.x);
	m_vertices.push_back(color.y);
	m_vertices.push_back(color.z);
	m_vertices.push_back(color.w);
}

void DebugDraw::dot(const glm::vec3& pos, const glm::vec4& color, float size) {
	const float hs = size / 2.0f;
	line(pos + glm::vec3(-hs, 0.0f, 0.0f), pos + glm::vec3(hs, 0.0f, 0.0f), color);
	line(pos + glm::vec3(0.0f, -hs, 0.0f), pos + glm::vec3(0.0f, hs, 0.0f), color);
	line(pos + glm::vec3(-hs, 0.0f, 0.0f), pos + glm::vec3(0.0f, -hs, 0.0f), color);
	line(pos + glm::vec3(hs, 0.0f, 0.0f), pos + glm::vec3(0.0f, -hs, 0.0f), color);
	line(pos + glm::vec3(-hs, 0.0f, 0.0f), pos + glm::vec3(0.0f, hs, 0.0f), color);
	line(pos + glm::vec3(hs, 0.0f, 0.0f), pos + glm::vec3(0.0f, hs, 0.0f), color);
}

static glm::vec3 getArcPoint(u32 i, float radius, float az, float start, float end, u32 segments) {
	if (end < start) {
		std::swap(end, start);
	}
	const float angleIncrement = std::abs(end - start) / segments;
	float angle = float(i) * angleIncrement + start;
	float cx = std::cos(angle) * radius;
	float cy = std::sin(angle) * radius;
	return glm::vec3(cx, cy, az);
}

void DebugDraw::circle(const glm::vec3& pos, float radius, const glm::vec4& color) {
	const u32 segs = 32;
	const glm::vec4 col{ color.r, color.g, color.b, color.a };
	glm::vec3 prev = getArcPoint(0, radius, 0.0f, 0.0f, glm::pi<float>() * 2.0f, segs) + pos;

	for (u32 i = 1; i <= segs; i++) {
		glm::vec3 cvec = getArcPoint(i, radius, 0.0f, 0.0f, glm::pi<float>() * 2.0f, segs) + pos;
		DebugDraw::get().line(prev, cvec, col);
		prev = cvec;
	}
}

DebugDraw& DebugDraw::get() {
	if (s_instance == nullptr) {
		s_instance = UPtr<DebugDraw>(new DebugDraw());
	}
	return *s_instance.get();
}

void PhysicsDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	b2Vec2 prev = vertices[0];
	const glm::vec4 col{ color.r, color.g, color.b, color.a };
	for (int32 i = 1; i < vertexCount; i++) {
		b2Vec2 vt = vertices[i];
		DebugDraw::get().line(glm::vec3(prev.x, prev.y, 0.0f), glm::vec3(vt.x, vt.y, 0.0f), col);
		prev = vt;
	}
	DebugDraw::get().line(glm::vec3(prev.x, prev.y, 0.0f), glm::vec3(vertices[0].x, vertices[0].y, 0.0f), col);
}

void PhysicsDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	DrawPolygon(vertices, vertexCount, color);
}

void PhysicsDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
	DebugDraw::get().circle(glm::vec3(center.x, center.y, 0.0f), radius, glm::vec4(color.r, color.g, color.b, color.a));
}

void PhysicsDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
}

void PhysicsDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
	const glm::vec4 col{ color.r, color.g, color.b, color.a };
	DebugDraw::get().line(glm::vec3(p1.x, p1.y, 0.0f), glm::vec3(p2.x, p2.y, 0.0f), col);
}

void PhysicsDebugDraw::DrawTransform(const b2Transform& xf) {
}
