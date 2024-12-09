#include "ShaderProgram.hpp"

#include <fstream>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>

// set uniform according to name
// https://docs.gl/gl4/glUniform

ShaderProgram::ShaderProgram(const std::filesystem::path& VS_file,
                             const std::filesystem::path& FS_file) {
    std::vector<GLuint> shader_ids;

    shader_ids.push_back(compile_shader(VS_file, GL_VERTEX_SHADER));
    shader_ids.push_back(compile_shader(FS_file, GL_FRAGMENT_SHADER));

    ID = link_shader(shader_ids);
}

/* Get location and write error to console */
GLuint ShaderProgram::getUniformLocation(const std::string& name) {
	auto loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) {
		std::cerr << "no uniform with name:" << name << '\n';
	}
	return loc;
}

inline void ShaderProgram::setUniform(const std::string& name, const float val) {
    auto loc = glGetUniformLocation(ID, name.c_str());
    glUniform1f(loc, val);
}

inline void ShaderProgram::setUniform(const std::string& name, const int val) {
	auto loc = glGetUniformLocation(ID, name.c_str());
	glUniform1i(loc, val);
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::vec3 val) {
    auto loc = glGetUniformLocation(ID, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(val));
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::vec4 in_vec4) {
    auto loc = glGetUniformLocation(ID, name.c_str());
	glUniform4fv(loc, 1, glm::value_ptr(in_vec4));
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::mat3 val) {
    auto loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

inline void ShaderProgram::setUniform(const std::string& name, const glm::mat4 val) {
    auto loc = glGetUniformLocation(ID, name.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

std::string ShaderProgram::getShaderInfoLog(const GLuint obj) {
    // TODO: implement
	return std::string();
}

std::string ShaderProgram::getProgramInfoLog(const GLuint obj) {
    // TODO: implement, check for result, print info & throw error (if any)
	return std::string();
}

GLuint ShaderProgram::compile_shader(const std::filesystem::path& source_file, const GLenum type) {
    GLuint shader_h = glCreateShader(type);

    std::string shader_code = textFileRead(source_file);
    const char* shader_code_cstr = shader_code.c_str();
    glShaderSource(shader_h, 1, &shader_code_cstr, nullptr);
    glCompileShader(shader_h);

    GLint success;
    glGetShaderiv(shader_h, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::cerr << "Error compiling shader: " << source_file << std::endl;
        std::cerr << getShaderInfoLog(shader_h) << std::endl;
        glDeleteShader(shader_h);
        throw std::runtime_error("Shader compilation failed.");
    }

    return shader_h;
}

/* Link all shader IDs to final program */
GLuint ShaderProgram::link_shader(const std::vector<GLuint> shader_ids) {
    GLuint prog_h = glCreateProgram();

    for (const auto& id : shader_ids) glAttachShader(prog_h, id);

    glLinkProgram(prog_h);

    GLint success;
    glGetProgramiv(prog_h, GL_LINK_STATUS, &success);
    if (!success) {
        std::cerr << "Error linking shader program." << std::endl;
        std::cerr << getProgramInfoLog(prog_h) << std::endl;
        glDeleteProgram(prog_h);
        throw std::runtime_error("Shader linking failed.");
    } else {
		std::cout << "Shader program linked successfully." << std::endl;
	}

    for (const auto& id : shader_ids) {
		glDeleteShader(id);
	}

    return prog_h;
}

std::string ShaderProgram::textFileRead(const std::filesystem::path& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
		throw std::runtime_error("Error opening file: " + filename.string());
	}
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}
