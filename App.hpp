#pragma once

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif
#include <GLFW/glfw3.h>

#include "assets.hpp"
#include "Model.h"

// our application class 
class App {
public:
    App(); // default constructor, called on app instance definition

    // public methods
    bool init(void);
    void init_imgui();
    int run(void);
    void destroy(void);

    color triangle_color{ 1.0f, 0.0f, 0.0f, 1.0f };
    void update_triangle_color(float delta);

    ~App(); //default destructor, called on app instance destruction
private:
    // GL
    GLFWwindow* window = nullptr;
    bool is_vsync_on = true;
    bool show_imgui = true;

    //new GL stuff
    GLuint shader_prog_ID{ 0 };
    GLuint VBO_ID{ 0 };
    GLuint VAO_ID{ 0 };

    // triangle vertices
    std::vector<vertex> triangle_vertices = {
        {{0.0f,  0.5f,  0.0f}},
        {{0.5f, -0.5f,  0.0f}},
        {{-0.5f, -0.5f,  0.0f}}
    };

    // triangle mesh
    std::unique_ptr<Mesh> triangleMesh;

    // list of Models
    std::vector<Model> models;

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

    //callbacks
    static void glfw_error_callback(int error, const char* description);
    static void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

};
