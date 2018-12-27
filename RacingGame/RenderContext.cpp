#include "RenderContext.h"

#include "Utils.h"
#include "glm/gtc/matrix_transform.hpp"

RenderContext::RenderContext() {
	m_lightCount = 0;

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);
	glGenVertexArrays(1, &m_vao);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void*) offsetof(Vertex, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*) offsetof(Vertex, normal));
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (void*) offsetof(Vertex, tangent));
	glVertexAttribPointer(3, 2, GL_FLOAT, false, sizeof(Vertex), (void*) offsetof(Vertex, texCoord));
	glVertexAttribPointer(4, 4, GL_FLOAT, false, sizeof(Vertex), (void*) offsetof(Vertex, color));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

	glBindVertexArray(0);

	const String VS =
#include "uber.vert"
		;

	const String FS =
#include "uber.frag"
		;

	m_shader = ShaderFactory::create()
		.addSource(ShaderProgram::VertexShader, VS)
		.addSource(ShaderProgram::FragmentShader, FS)
		.link();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

RenderContext::~RenderContext() {
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
	glDeleteVertexArrays(1, &m_vao);
}

void RenderContext::begin() {
	m_drawables.clear();
}

void RenderContext::end() {
	updateBufferData();

	m_shader.use();

	m_shader.get("uView").set(m_view);
	m_shader.get("uProj").set(m_projection);
	m_shader.get("uModel").set(glm::mat4(1.0f));
	m_shader.get("uHasNormal").set(false);
	m_shader.get("uHasSpecular").set(false);
	m_shader.get("uHasEnv").set(false);

	u32 startSlot = 0;
	m_shader.get("uAmbient").set(m_ambient);
	if (m_environment.id() > 0) {
		m_environment.bind(startSlot);
		m_shader.get("uEnv").set(startSlot);
		m_shader.get("uHasEnv").set(true);
		startSlot++;
	}

	m_shader.get("uLightCount").set(m_lightCount);
	for (u32 i = 0; i < m_lightCount; i++) {
		Light li = m_lights[i];
		const String light = Utils::concat("uLights[", i, "].");
		m_shader.get(light + "position").set(li.position);
		m_shader.get(light + "direction").set(li.direction);
		m_shader.get(light + "color").set(li.color);
		m_shader.get(light + "intensity").set(li.intensity);
		m_shader.get(light + "radius").set(li.radius);
		m_shader.get(light + "spotCutoff").set(li.spotCutoff);
		m_shader.get(light + "type").set(u32(li.type));
	}

	glBindVertexArray(m_vao);
	for (Batch b : m_batches) {
		if (b.transform) {
			m_shader.get("uModel").set(b.modelMatrix);
		}

		u32 slot = startSlot;
		if (b.color.id() > 0) {
			b.color.bind(slot);
			m_shader.get("uColor").set(slot);
			slot++;
		}
		if (b.normal.id() > 0) {
			b.normal.bind(slot);
			m_shader.get("uNormal").set(slot);
			m_shader.get("uHasNormal").set(true);
			slot++;
		}
		if (b.specular.id() > 0) {
			b.specular.bind(slot);
			m_shader.get("uSpecular").set(slot);
			m_shader.get("uHasSpecular").set(true);
			slot++;
		}

		glDrawElements(
			GL_TRIANGLES,
			b.length,
			GL_UNSIGNED_INT,
			(void*) (b.offset * 4)
		);
	}
	glBindVertexArray(0);

	m_batches.clear();
	m_lightCount = 0;
}

void RenderContext::submit(
	const Mesh& mesh,
	const glm::mat4& modelMatrix,
	Texture2D color,
	Texture2D normal,
	Texture2D specular)
{
	Drawable d{};
	d.color = color;
	d.normal = normal;
	d.specular = specular;
	d.transform = true;
	d.modelMatrix = modelMatrix;
	d.vertices = mesh.vertices();
	d.indices = mesh.indices();

	m_drawables.push_back(d);
}

void RenderContext::submitSprite(
	Texture2D color,
	Texture2D normal,
	Texture2D specular,
	glm::vec4 vcolor)
{
	glm::vec4 region = m_cursor.m_region;
	glm::vec3 pos = m_cursor.m_position;
	glm::vec3 ori = m_cursor.m_origin;
	glm::vec2 scale = m_cursor.m_scale;
	float rotation = m_cursor.m_rotation;

	float asp = float(color.width()) / float(color.height());
	float w = color.width() >= color.height() ? 1.0f : asp;
	float h = color.height() >= color.width() ? 1.0f : asp;

	float u0 = region.x;
	float u1 = region.x + region.z;
	float v0 = region.y;
	float v1 = region.y + region.w;

	Vertex verts[] = {
		{ { 0, 0, 0 },  { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u0, v1 }, vcolor },
		{ { w, 0, 0 },  { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u1, v1 }, vcolor },
		{ { w, h, 0 },  { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u1, v0 }, vcolor },
		{ { 0, h, 0 },  { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u0, v0 }, vcolor }
	};
	u32 inds[] = { 0, 1, 2, 0, 2, 3 };

	for (Vertex& v : verts) {
		v.position -= ori * glm::vec3(w, h, 1.0f);
	}

	Mesh spr{};
	spr.vertices(ToVector(Vertex, verts));
	spr.indices(ToVector(u32, inds));

	glm::mat4 T = glm::translate(glm::mat4(1.0f), pos);
	if (rotation != 0.0f) {
		T *= glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 0, 1));
	}
	if (scale.x != 1.0f || scale.y != 1.0f) {
		T *= glm::scale(glm::mat4(1.0f), glm::vec3(scale, 1.0f));
	}

	spr.transform(T);
	spr.calculateTangents();

	Drawable d{};
	d.color = color;
	d.normal = normal;
	d.specular = specular;
	d.transform = false;
	d.modelMatrix = glm::mat4(1.0f);
	d.vertices = spr.vertices();
	d.indices = spr.indices();

	m_drawables.push_back(d);
}

void RenderContext::submitSunLight(
	const glm::vec3& direction,
	const glm::vec3& color,
	float intensity)
{
	Light& light = m_lights[m_lightCount % MAX_LIGHTS];
	light.type = Light::Sun;
	light.color = color;
	light.direction = direction;
	light.intensity = intensity;
	m_lightCount++;
	m_lightCount = m_lightCount % MAX_LIGHTS;
}

void RenderContext::submitPointLight(
	const glm::vec3& position,
	const glm::vec3& color,
	float intensity,
	float radius) 
{
	Light& light = m_lights[m_lightCount % MAX_LIGHTS];
	light.type = Light::Point;
	light.position = position;
	light.color = color;
	light.intensity = intensity;
	light.radius = radius;
	m_lightCount++;
	m_lightCount = m_lightCount % MAX_LIGHTS;
}

void RenderContext::submitSpotLight(
	const glm::vec3& position,
	const glm::vec3& direction,
	const glm::vec3& color,
	float intensity,
	float radius,
	float cutOff)
{
	Light& light = m_lights[m_lightCount % MAX_LIGHTS];
	light.type = Light::Spot;
	light.position = position;
	light.direction = direction;
	light.color = color;
	light.intensity = intensity;
	light.radius = radius;
	light.spotCutoff = cutOff;
	m_lightCount++;
	m_lightCount = m_lightCount % MAX_LIGHTS;
}

void RenderContext::updateBufferData() {
	if (m_drawables.empty()) return;

	std::sort(
		m_drawables.begin(), m_drawables.end(),
		[](const Drawable& a, const Drawable& b) -> bool {
			return a.color.id() < b.color.id();
		}
	);

	Vec<Vertex> vertices;
	Vec<u32> indices;

	Drawable first = m_drawables[0];
	vertices.insert(vertices.end(), first.vertices.begin(), first.vertices.end());
	indices.insert(indices.end(), first.indices.begin(), first.indices.end());

	m_batches.emplace_back(
		first.modelMatrix,
		0,
		first.indices.size(),
		first.color, first.normal, first.specular,
		first.transform
	);

	u32 offset = 0;
	u32 indexOffset = first.vertices.size();

	for (u32 i = 1; i < m_drawables.size(); i++) {
		Drawable prev = m_drawables[i - 1];
		Drawable curr = m_drawables[i];
		if (prev.color.id() != curr.color.id() ||
			prev.normal.id() != curr.normal.id() ||
			prev.specular.id() != curr.specular.id() ||
			prev.transform != curr.transform)
		{
			offset += m_batches.back().length;
			m_batches.emplace_back(
				curr.modelMatrix,
				offset,
				curr.indices.size(),
				curr.color, curr.normal, curr.specular,
				curr.transform
			);
		} else {
			m_batches.back().length += u32(curr.indices.size());
		}

		vertices.insert(vertices.end(), curr.vertices.begin(), curr.vertices.end());
		for (u32 id : curr.indices) {
			indices.push_back(id + indexOffset);
		}

		indexOffset += u32(curr.vertices.size());
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	if (vertices.size() > m_vboSize) {
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
		m_vboSize = vertices.size();
	} else {
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	if (indices.size() > m_eboSize) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_DYNAMIC_DRAW);
		m_eboSize = indices.size();
	} else {
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(u32), indices.data());
	}
}
