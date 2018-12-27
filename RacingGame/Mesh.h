#ifndef MESH_H
#define MESH_H

#include "glad.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Factory.h"
#include "Collections.h"

#pragma pack(push, 1)
struct Vertex {
	glm::vec3 position, normal, tangent;
	glm::vec2 texCoord;
	glm::vec4 color;
};
#pragma pack(pop)

class Mesh {
public:
	Mesh() = default;
	~Mesh() = default;

	void vertices(const Vec<Vertex>& verts) { m_vertices = verts; }
	void indices(const Vec<u32>& inds) { m_indices = inds; }
	Vec<Vertex> vertices() const { return m_vertices; }
	Vec<u32> indices() const { return m_indices; }

	void calculateTangents();
	void calculateNormals();

	void transform(const glm::mat4& modelMatrix);

protected:
	Vec<Vertex> m_vertices;
	Vec<u32> m_indices;
};

#endif // MESH_H