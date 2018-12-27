R"(
#version 330 core
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vTangent;
layout (location = 3) in vec2 vTexCoord;
layout (location = 4) in vec4 vColor;

uniform mat4 uProj;
uniform mat4 uView;
uniform mat4 uModel = mat4(1.0);

out DATA {
	vec4 color;
	vec4 position;
	vec3 normal;
	vec3 tangent;
	vec3 eye;
	vec2 uv;
	mat3 tbn;
} VSOut;

void main() {
	vec4 pos = uModel * vec4(vPosition, 1.0);
	gl_Position = uProj * uView * pos;

	VSOut.eye = normalize(-(uView * pos)).xyz;

	VSOut.color = vColor;
	VSOut.position = pos;
	VSOut.uv = vTexCoord;
	
	const vec3 NORMAL = vec3(0.0, 0.0, 1.0);
	VSOut.normal = NORMAL;
	VSOut.tangent = normalize(vTangent - dot(vTangent, VSOut.normal) * VSOut.normal);

	vec3 b = cross(VSOut.tangent, VSOut.normal);
    VSOut.tbn = mat3(VSOut.tangent, b, VSOut.normal);
}
)"