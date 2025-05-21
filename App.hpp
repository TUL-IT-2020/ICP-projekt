#ifndef APP_HPP
#define APP_HPP

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <string>
#include <opencv2/opencv.hpp>

#include "assets.hpp"
#include "Model.hpp"
#include "camera.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Bullet.hpp"

enum class TextureFilter {
    Nearest,       // Nearest neighbor - rychlé, ale nekvalitní
    Bilinear,      // Bilineární filtrování - kvalitnější, ale pomalejší
    TrilinearMipmap // Trilineární filtrování s MIPMAP - nejlepší kvalita, vyšší paměťové nároky
};

// our application class 
class App {
public:
    App(); // default constructor, called on app instance definition

    // public methods
    bool init(void);
    void init_imgui();
    int run(void);
    void destroy(void);
    
    static GLuint gen_tex(cv::Mat& image, TextureFilter filter);

    ~App(); //default destructor, called on app instance destruction
private:
    // Map
    Map map;
	std::unordered_map<std::string, Model> map_2_model_dict;
    // list of bullets
    std::vector<Bullet> bullets;
    bool CheckHitboxes(glm::vec3 movement);

    // camera related 
    Camera camera;
    Player player;
    // remember last cursor position, move relative to that in the next frame
    double cursorLastX{ 0 };
    double cursorLastY{ 0 };

    // GL
    GLFWwindow* window = nullptr;
    bool is_vsync_on = true;
    bool show_imgui = true;

    // list of Models
    std::unordered_map<std::string, ShaderProgram> shader_cache;
    std::unordered_map<std::string, Model> model_cache;
    std::vector<Model> models;
    //ShaderProgram shader;

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

    // callbacks
    void update_projection_matrix(void);
    void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void fbsize_callback(GLFWwindow* window, int width, int height);
    // static callbacks
    static void glfw_error_callback(int error, const char* description);
    static void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

protected:
	// projection related variables    
    int width{0}, height{0};
    float fov = 60.0f;
    // store projection matrix here, update only on callbacks
    glm::mat4 projection_matrix = glm::identity<glm::mat4>();

	// all objects of the scene
    std::unordered_map<std::string, Model> scene; 
};

#endif // APP_HPP