#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <GL/glew.h>
#include <glm/glm.hpp>

class ShaderProgram {
public:
	// you can add more constructors for pipeline with GS, TS etc.
	ShaderProgram(void) = default; //does nothing
	ShaderProgram(const std::filesystem::path & VS_file, const std::filesystem::path & FS_file);

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
	GLuint getUniformLocation(const std::string& name);
	std::string getShaderInfoLog(const GLuint obj);   // TODO: check for shader compilation error; if any, print compiler output  
	std::string getProgramInfoLog(const GLuint obj);  // TODO: check for linker error; if any, print linker output

	GLuint compile_shader(const std::filesystem::path & source_file, const GLenum type);
	GLuint link_shader(const std::vector<GLuint> shader_ids);
    std::string textFileRead(const std::filesystem::path & filename);
};

