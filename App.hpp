#pragma once

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif
#include <unordered_map>

#include <GLFW/glfw3.h>

#include "assets.hpp"
#include "Model.hpp"
#include "camera.hpp"

// our application class 
class App {
public:
    App(); // default constructor, called on app instance definition

    // public methods
    bool init(void);
    void init_imgui();
    int run(void);
    void destroy(void);
    
	std::filesystem::path vertex_shader_path = "resources/shaders/basic_core.vert";
	std::filesystem::path fragment_shader_path = "resources/shaders/basic_core.frag";

    color triangle_color{ 1.0f, 0.0f, 0.0f, 1.0f };
    void update_triangle_color(float delta);
    int width{0}, height{0};
    float fov = 60.0f;
    // store projection matrix here, update only on callbacks
    glm::mat4 projection_matrix = glm::identity<glm::mat4>();

	// all objects of the scene
    std::unordered_map<std::string, Model> scene;

    ~App(); //default destructor, called on app instance destruction
private:
   // int width, height;
    //float fov;
    //glm::mat4 projection_matrix;
    // camera related 
    Camera camera;
    // remember last cursor position, move relative to that in the next frame
    double cursorLastX{ 0 };
    double cursorLastY{ 0 };
   
    // GL
    GLFWwindow* window = nullptr;
    bool is_vsync_on = true;
    bool show_imgui = true;

    // list of Models
    std::vector<Model> models;
    ShaderProgram shader;

    void thread_code(void);

    void init_opencv();
    void init_glew(void);
    void init_glfw(void);
    void init_gl_debug();
    void init_assets(void);

    void print_opencv_info();
    void print_glfw_info(void);
    void print_glm_info();
    void print_gl_info();
    void update_projection_matrix(void);
    //callbacks
    static void glfw_error_callback(int error, const char* description);
    static void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
    static void scroll_callback(GLFWwindow * window, double xoffset, double yoffset);
    static void fbsize_callback(GLFWwindow* window, int width, int height);
    void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    

};
