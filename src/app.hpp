#ifndef APP_HPP
    #define APP_HPP

    #include <iostream>

    #include <glad/glad.h>
    #include <GLFW/glfw3.h>

    #include "event.hpp"
    #include "controls/widget.hpp"

    #include "renderer/drawing_context.hpp"

    void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    void processInput(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }
    
    class Application {
        public:
            int id_counter = 0;
            GLFWwindow *win;
            DrawingContext *dc; // TODO change to drawing context
            Color bg = {155, 155, 155, 255};
            Widget *main_widget;
            Size m_size;
            Widget *last_widget_with_mouse = nullptr;

            Application(const char* title = "Application", Size size = Size { 400, 400 }) {
                this->m_size = size;

                glfwInit();
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                #ifdef __APPLE__
                    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
                #endif

                win = glfwCreateWindow(size.width, size.height, title, NULL, NULL);
                if (!this->win) {
                    println("Failed to create GLFW window");
                    glfwTerminate();
                }
                glfwMakeContextCurrent(this->win);
                glfwSetFramebufferSizeCallback(this->win, framebuffer_size_callback);

                if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                    println("Failed to initialize GLAD");
                }

                dc = new DrawingContext();
            }

            ~Application() {
                // TODO recursively delete
                delete main_widget;
            }

            void draw() {
                this->main_widget->draw(this->dc, Rect { 0, 0, this->m_size.width, this->m_size.height });
            }

            void set_main_widget(Widget *widget) {
                this->main_widget = widget;
            }

            void show() {
                this->draw();
                this->dc->swap_buffer(this->win);
            }

            int next_id() {
                return this->id_counter++;
            }

            void run() {
                while (!glfwWindowShouldClose(this->win)) {
                    // this->last_widget_with_mouse = this->main_widget->propagate_mouse_event(this->last_widget_with_mouse, MouseEvent(event.button));
                    processInput(this->win);
                    this->dc->clear(this->win);
                    
                    int width, height;
                    glfwGetWindowSize(this->win, &width, &height);
            
                    this->dc->set_projection(glm::ortho(
                        0.0f, static_cast<float>(width),
                        static_cast<float>(height), 0.0f,
                        -1.0f, 1.0f
                    ));
                    
                    this->show();
                    glfwWaitEvents();
                }
                glfwTerminate();
            }
    };
#endif