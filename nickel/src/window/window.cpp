#include "window/window.hpp"

namespace nickel {

Window::Window(const std::string& title, int width, int height) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config::GLMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config::GLMinorVersion);

    window_ = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window_) {
        LOGE("GLFW", "create window failed");
        glfwTerminate();
    } else {
        glfwMakeContextCurrent(window_);
        if (gladLoadGL() == 0) {
            LOGE("GLAD", "load opengl ", config::GLMajorVersion, ".",
                 config::GLMinorVersion, " failed");
            glfwTerminate();
        }
        int w, h;
        glfwGetFramebufferSize(window_, &w, &h);
        GL_CALL(glViewport(0, 0, w, h));
        glfwSetFramebufferSizeCallback(
            window_, +[](GLFWwindow* window, int width, int height) {
                GL_CALL(glViewport(0, 0, width, height));
            });
    }
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(window_);
}

void Window::SwapBuffer() const {
    glfwSwapBuffers(window_);
}

Window::~Window() {
    glfwDestroyWindow(window_);
}

WindowBuilder WindowBuilder::FromConfig(const std::string& filename) {
    WindowBuilder builder = Default();
    toml::parse_result result = toml::parse_file(filename);
    if (!result) {
        LOGE(ConfigErr, "create window from ", filename,
             " failed, use default config");
    } else {
        auto& tbl = result.table();
        if (auto window_tbl = tbl["window"].as_table(); !window_tbl) {
            LOGW(ConfigErr, "window config invalid, please check ", filename);
        } else {
            toml::table& window = *window_tbl;
            if (auto title = window["title"].value<std::string>();
                title.has_value()) {
                builder.Title(title.value());
            }
            if (auto size = window["size"].as_array();
                size && size->size() == 2) {
                builder.Size(
                    {static_cast<float>(size->at(0).value<int>().value()),
                     static_cast<float>(size->at(1).value<int>().value())});
            }
        }
    }

    return builder;
}

WindowBuilder WindowBuilder::Default() {
    return WindowBuilder().Title("nickel engine").Size({640, 480});
}

}  // namespace nickel