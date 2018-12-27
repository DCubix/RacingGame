#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "Factory.h"
#include "Collections.h"
#include "glad.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

struct Uniform {
	friend class ShaderProgram;

	u32 loc;

	void set(bool value) { glUniform1i(loc, value ? 1 : 0); }
	void set(u32 value) { glUniform1i(loc, value); }
	void set(int value) { glUniform1i(loc, value); }
	void set(float value) { glUniform1f(loc, value); }
	void set(const glm::vec2& value) { glUniform2f(loc, value.x, value.y); }
	void set(const glm::vec3& value) { glUniform3f(loc, value.x, value.y, value.z); }
	void set(const glm::vec4& value) { glUniform4f(loc, value.x, value.y, value.z, value.w); }
	void set(const glm::mat4& value) { glUniformMatrix4fv(loc, 1, false, glm::value_ptr(value)); }

private:
	Uniform(u32 loc) : loc(loc) {}
};

class ShaderProgram {
	friend class Factory<ShaderProgram>;
public:
	enum ShaderType {
		VertexShader = GL_VERTEX_SHADER,
		FragmentShader = GL_FRAGMENT_SHADER
	};

	ShaderProgram() = default;
	~ShaderProgram() = default;

	ShaderProgram& addSource(ShaderType type, const String& src);
	ShaderProgram& link();

	void use();

	Uniform get(const String& name) { return Uniform(getUniformLocation(name)); }

	u32 getUniformLocation(const String& name);

protected:
	GLuint m_program;
	UMap<String, u32> m_uniforms;
};

template <>
class Factory<ShaderProgram> {
public:
	static ShaderProgram& create() {
		ShaderProgram shader{};
		shader.m_program = glCreateProgram();
		s_shaderPrograms.push_back(shader);
		return s_shaderPrograms.back();
	}

	static void release() {
		for (auto&& prog : s_shaderPrograms) {
			glDeleteProgram(prog.m_program);
		}
		s_shaderPrograms.clear();
	}

private:
	static Vec<ShaderProgram> s_shaderPrograms;
};

using ShaderFactory = Factory<ShaderProgram>;

#endif // SHADER_PROGRAM_H