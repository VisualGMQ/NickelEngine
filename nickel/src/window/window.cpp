#include "window/window.hpp"
#include "refl/window.hpp"
#include "config/config.hpp"

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

WindowBuilder WindowBuilder::FromConfig(std::string_view filename) {
    WindowBuilder::Data data = WindowBuilder::Data::Default();
    auto parseResult = toml::parse_file(filename);
    if (parseResult.failed() || parseResult.table().find("window") == parseResult.table().end()) {
        LOGW(log_tag::Config, "Read window config from ", filename, " failed. Use default config. Error: ", parseResult.error());
    } else {
        data = mirrow::serd::srefl::deserialize<WindowBuilder::Data>(*parseResult.table()["window"].as_table());
    }

    return WindowBuilder(data);
}

WindowBuilder::Data WindowBuilder::Data::Default() {
    return {
        config::DefaultWindowTitle,
        {config::DefaultWindowWidth, config::DefaultWindowHeight}
    };
}

WindowBuilder WindowBuilder::Default() {
    return WindowBuilder(WindowBuilder::Data::Default());
}

}  // namespace nickel