#include "Mesh.h"

void Mesh::calculateTangents() {
	for (u32 i = 0; i < m_indices.size(); i += 3) {
		Vertex& v0 = m_vertices[m_indices[i + 0]];
		Vertex& v1 = m_vertices[m_indices[i + 1]];
		Vertex& v2 = m_vertices[m_indices[i + 2]];

		glm::vec3 edge1 = v2.position - v0.position;
		glm::vec3 edge2 = v1.position - v0.position;
		glm::vec2 deltaUV1 = v2.texCoord - v0.texCoord;
		glm::vec2 deltaUV2 = v1.texCoord - v0.texCoord;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 tang{};
		tang.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tang.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tang.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tang = glm::normalize(tang);
		
		v0.tangent += tang;
		v1.tangent += tang;
		v2.tangent += tang;
	}
	
	for (Vertex& v : m_vertices) {
		v.tangent = glm::normalize(v.tangent);
	}
}

void Mesh::calculateNormals() {
	for (u32 i = 0; i < m_indices.size(); i += 3) {
		Vertex& v0 = m_vertices[m_indices[i + 0]];
		Vertex& v1 = m_vertices[m_indices[i + 1]];
		Vertex& v2 = m_vertices[m_indices[i + 2]];

		glm::vec3 edge1 = v2.position - v0.position;
		glm::vec3 edge2 = v1.position - v0.position;
		glm::vec3 n = glm::normalize(glm::cross(edge1, edge2));

		v0.normal += n;
		v1.normal += n;
		v2.normal += n;
	}

	for (Vertex& v : m_vertices) {
		v.normal = glm::normalize(v.normal);
	}
}

void Mesh::transform(const glm::mat4& modelMatrix) {
	for (Vertex& v : m_vertices) {
		v.position = glm::vec3(modelMatrix * glm::vec4(v.position, 1.0f));
	}
}