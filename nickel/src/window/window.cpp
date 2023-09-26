#include "window/window.hpp"
#include "config/config.hpp"
#include "refl/window.hpp"

namespace nickel {

Window::Window(const std::string& title, int width, int height): title_(title) {
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
    }
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(window_);
}

void Window::SwapBuffer() const {
    glfwSwapBuffers(window_);
}

cgmath::Vec2 Window::Size() const {
    int w, h;
    glfwGetWindowSize(window_, &w, &h);
    return {static_cast<float>(w), static_cast<float>(h)};
}

void Window::Resize(int w, int h) {
    glfwSetWindowSize(window_, w, h);
}

void Window::SetTitle(const std::string& title) {
    glfwSetWindowTitle(window_, title.c_str());
    title_ = title;
}

Window::~Window() {
    glfwDestroyWindow(window_);
}

WindowBuilder WindowBuilder::FromConfigFile(std::string_view filename) {
    WindowBuilder::Data data = WindowBuilder::Data::Default();
    auto parseResult = toml::parse_file(filename);
    if (parseResult.failed() ||
        parseResult.table().find("window") == parseResult.table().end()) {
        LOGW(log_tag::Config, "Read window config from ", filename,
             " failed. Use default config. Error: ", parseResult.error());
    } else {
        mirrow::serd::srefl::deserialize<WindowBuilder::Data>(
            *parseResult.table()["window"].as_table(), data);
    }

    return WindowBuilder(data);
}

WindowBuilder WindowBuilder::FromConfig(const toml::table& tbl) {
    Assert(tbl.is_table(), "window config must be a table");
    WindowBuilder::Data data = WindowBuilder::Data::Default();
    mirrow::serd::srefl::deserialize<WindowBuilder::Data>(tbl, data);
    return WindowBuilder(data);
}

WindowBuilder::Data WindowBuilder::Data::Default() {
    return {
        std::string{config::DefaultWindowTitle},
        {config::DefaultWindowWidth, config::DefaultWindowHeight}
    };
}

WindowBuilder WindowBuilder::Default() {
    return WindowBuilder(WindowBuilder::Data::Default());
}

}  // namespace nickel