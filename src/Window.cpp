#include "edw/Window.h"
#include <stdexcept>



namespace edw{
    Window::Window(){
        init();
        setupCallbacks();
        input.addJoystick(GLFW_JOYSTICK_1);
    }

    Window::~Window(){
        close();
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    void Window::init(){
        if(!glfwInit()){
            throw std::runtime_error("Failed to initialize GLFW!");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_DEPTH_BITS, 24); 

        window = glfwCreateWindow(size.width, size.height, "", nullptr, nullptr);

        if(!window){
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window!");
        }

        glfwMakeContextCurrent(window); 
        glfwSwapInterval(1);
   }


    void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
        auto* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if(win) win->input.onKey(key, scancode, action, mods);   
    }

    void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
        auto* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if(win) win->input.onMouseButton(button, action, mods);
    }

    void Window::cursorCallback(GLFWwindow* window, double xPos, double yPos){
        auto* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if(win) win->input.onCursorMove(xPos, yPos);
    }

    void Window::scrollCallback(GLFWwindow* window, double xOffset, double yOffset){
        auto* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if(win) win->input.onScroll(xOffset, yOffset);
    }



    void Window::setupCallbacks(){
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetCursorPosCallback(window, cursorCallback);
        glfwSetScrollCallback(window, scrollCallback);
    }
    
    void Window::close(){
        glfwSetWindowShouldClose(window, GL_TRUE);
    }   

    void Window::show(){
        for(const auto& behavior: initBehaviors) behavior();

        if(!window) return;
        while (!glfwWindowShouldClose(window)){
            
            for(const auto& behavior: frameBehaviors) behavior();
            
            input.processJoystick();

            glfwSwapBuffers(window);            
            glfwPollEvents();
        }
    
    }


    void Window::setFullScreen(bool value){
        if(value == isFullScreen()) return;
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        if (value) {
            // Save the windowed mode parameters
            glfwGetWindowPos(window, &position.x, &position.y);
            glfwGetWindowSize(window, &size.width, &size.height);

            // Switch to fullscreen
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            // Restore the windowed mode parameters
            glfwSetWindowMonitor(window, nullptr, position.x, position.y, size.width, size.height, 0);
        }

        fullScreen = value;
    }
}