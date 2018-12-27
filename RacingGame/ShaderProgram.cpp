#include "ShaderProgram.h"

#include "Utils.h"
#include "Logger.h"
#include <numeric>

Vec<ShaderProgram> Factory<ShaderProgram>::s_shaderPrograms;

ShaderProgram& ShaderProgram::addSource(ShaderType type, const String& src) {
	const char* csrc = src.c_str();
	
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &csrc, nullptr);
	glCompileShader(shader);

	GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		char log[2048];
		glGetShaderInfoLog(shader, 2048, nullptr, log);

		Vec<String> infoLog = Utils::split(String(log), '\n');
		Vec<String> srcSplit = Utils::split(src, '\n');
		String newInfoLog = "";
		for (String line : infoLog) {
			newInfoLog += line + String("\n");
			if (line.substr(0, 5) == "ERROR") {
				String li = line.substr(9);
				if (li.substr(0, 3) != "ror") {
					li = li.substr(0, li.find_first_of(':'));
					int idx = std::stoi(li) - 1;
					newInfoLog += String("\t at: ") + srcSplit[idx] + String("\n\n");
				}
			}
		}
		LogError("\n", newInfoLog);
	} else {
		glAttachShader(m_program, shader);
	}
	glDeleteShader(shader);

	return *this;
}

ShaderProgram& ShaderProgram::link() {
	glLinkProgram(m_program);
	return *this;
}

void ShaderProgram::use() {
	glUseProgram(m_program);
}

u32 ShaderProgram::getUniformLocation(const String& name) {
	if (m_uniforms.find(name) == m_uniforms.end()) {
		int loc = glGetUniformLocation(m_program, name.c_str());
		if (loc != -1) {
			m_uniforms.insert({ name, u32(loc) });
		} else {
			LogWarning("Invalid uniform name: \"", name, "\"");
		}
	}
	return m_uniforms[name];
}
