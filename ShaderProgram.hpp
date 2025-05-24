#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <string>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <GL/glew.h>
#include <glm/glm.hpp>

class ShaderProgram {
public:
	// you can add more constructors for pipeline with GS, TS etc.
	ShaderProgram(void) = default; //does nothing
	ShaderProgram(const std::filesystem::path & VS_file, const std::filesystem::path & FS_file);
	bool hasUniform(const std::string& name); // <-- ADD THIS LINE
	void activate(void) {
        if (ID==currently_used)
            return;
        else {
            glUseProgram(ID);
            currently_used = ID;
        }
	};
	
	void deactivate(void) { 
		glUseProgram(0); 
        currently_used = 0; 
	};   // deactivate current shader program (i.e. activate shader no. 0)

	/* deallocate shader program */
	void clear(void) {
		deactivate();
		glDeleteProgram(ID);
		ID = 0;
	}

	GLuint getID(void) { return ID; }
    
    // set uniform according to name 
    // https://docs.gl/gl4/glUniform
    void setUniform(const std::string & name, const float val);
    void setUniform(const std::string & name, const int val);
    void setUniform(const std::string & name, const glm::vec3 val);
    void setUniform(const std::string & name, const glm::vec4 val);
    void setUniform(const std::string & name, const glm::mat3 val);
    void setUniform(const std::string & name, const glm::mat4 val);
    
private:
	GLuint ID{0}; // default = 0, empty shader
	static GLuint currently_used;
	std::unordered_map<std::string, GLuint> uniform_location_cache;

	GLuint getUniformLocation(const std::string& name);
	std::string getShaderInfoLog(const GLuint obj);
	std::string getProgramInfoLog(const GLuint obj);

	GLuint compile_shader(const std::filesystem::path & source_file, const GLenum type);
	GLuint link_shader(const std::vector<GLuint> shader_ids);
    std::string textFileRead(const std::filesystem::path & filename);
};

#endif // SHADERPROGRAM_HPP