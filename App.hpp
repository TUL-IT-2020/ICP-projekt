#ifndef APP_HPP
#define APP_HPP

#include <opencv2/opencv.hpp>
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <string>

#include "assets.hpp"
#include "Model.hpp"
#include "camera.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include "Bullet.hpp"
#include "Door.hpp"
#include "ShaderProgram.hpp"
#include "StatusBar.hpp"
#include "Light.hpp"

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
    
    cv::Point2f find_face(cv::Mat & frame);
	void draw_cross_relative(cv::Mat& img, cv::Point2f center_relative, int size);
	char webcam_to_movement(const cv::Point2f& center);
    
    static GLuint gen_tex(cv::Mat& image, TextureFilter filter);

    ~App(); //default destructor, called on app instance destruction
private:
    // Map
    Map map;
    int level{ 1 }; // level number
    bool load_new_level{ false };
	std::unordered_map<std::string, Model> map_2_model_dict;
    // list of bullets
    std::vector<Bullet> bullets;
    // list of lights
    const int MAX_LIGHTS = 10;
    std::vector<Light> lights;

    // camera related 
    Camera camera;
    // player related
    Player player;
    ShaderProgram directionalShader;
    std::shared_ptr<StatusBar> status_bar;
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
    std::vector<std::unique_ptr<Model>> models;
    //ShaderProgram shader;

    // webcam
    cv::VideoCapture capture;
    cv::CascadeClassifier face_cascade; // Face cascade classifier

	// thread controll variables
	std::mutex mtx;
	bool user_exit = false;
	bool cam_disconnected = false;
	bool process_images = true;
	char movement_from_webcam = 'n'; // 'n' for no movement_from_webcam, 'l' for left, 'r' for right.

	// thread shared variables
	bool new_coordinates = false;
	cv::Point2f center = cv::Point2f(0.0f, 0.0f);
	cv::Mat frame;

	// thread functions
	void camera_processing_thread();
    void thread_code(void);
    void UI_thread();

    bool CheckHitboxes(glm::vec3 movement);
    bool webcam_connected = true; // if webcam is not available, use keyboard input

    // init
    void init_opencv();
    void init_glew(void);
    void init_glfw(void);
    void init_gl_debug();
    void init_assets(void);
    void init_map_for_level_and_generate_scene(int level);
    bool webcam_init();
    void clasificator_init();

    // print info
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