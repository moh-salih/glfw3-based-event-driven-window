#ifndef EDW_WINDOW_H
#define EDW_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <functional>
#include <unordered_map>
#include <iostream>

namespace edw{


    class Input{
        void handleJoystickAnalog(int joystickID){
            if(!glfwJoystickPresent(joystickID)) return;


            int axesCount;
            const float * axes = glfwGetJoystickAxes(joystickID, &axesCount);
            if(axes){
                for(size_t i{}; i < axesCount; ++i){
                    auto it = analogCallbacks.find(i);
                    if(it != analogCallbacks.end()){
                        for(const auto& callback: it->second) callback(axes[i]);
                    }
                }
            }

        }

        void handleJoystickButtons(int joystickID){
            if(!glfwJoystickPresent(joystickID)) return;


            int buttonCount;
            const unsigned char * buttons = glfwGetJoystickButtons(joystickID, &buttonCount);
            for(int i{}; i < buttonCount; ++i){
                if(previousButtonState[joystickID][i] != buttons[i]){
                    if(buttons[i] == GLFW_PRESS){
                        auto it = joystickPressCallbacks.find(i);
                        if(it != joystickPressCallbacks.end()){
                            for(const JoystickCallback& callback: it->second) callback();
                        }    
                    }else if(buttons[i] == GLFW_RELEASE){
                        auto it = joystickReleaseCallbacks.find(i);
                        if(it != joystickReleaseCallbacks.end()){
                            for(const JoystickCallback& callback: it->second) callback();
                        }
                    }
                    previousButtonState[joystickID][i] = buttons[i];                    
                }
            }
        }

        void onKey(int key, int scancode, int action, int mods){
            if(action == GLFW_PRESS){
                auto it = keyPressCallbacks.find(key);
                if(it != keyPressCallbacks.end()){
                    for(const KeyCallback& callback: it->second) callback();
                }
            }else if(action == GLFW_RELEASE){
                auto it = keyReleaseCallbacks.find(key);
                if(it != keyReleaseCallbacks.end()){
                    for(const KeyCallback& callback: it->second) callback();
                }
            }
        }

        void onMouseButton(int button, int action, int mods){
            if(action == GLFW_PRESS && mousePressCallbacks.contains(button)){
                for(const auto& callback: mousePressCallbacks[button]) callback();
            }
            else if(action == GLFW_PRESS && mouseReleaseCallbacks.contains(button)){
                for(const auto& callback: mouseReleaseCallbacks[button]) callback();
            }
        }

        void onCursorMove(double xPos, double yPos){
            for(const auto& callback: cursorCallbacks) callback(xPos, yPos);
        }

        void onScroll(double xOffset, double yOffset){}
        
        void processJoystick(){
            for(const auto& joystickID: joysticks){
                handleJoystickAnalog(joystickID);
                handleJoystickButtons(joystickID);
            }
        }

    public:
        typedef std::function<void()> KeyCallback;
        typedef std::function<void()> JoystickCallback;
        typedef std::function<void(float)> AnalogCallback;
        typedef std::function<void(float, float)> CursorCallback;

        void addJoystick(int id){
            joysticks.push_back(id);
            previousButtonState[id] = std::vector<unsigned char>(16, GLFW_RELEASE);
            previousAxesState[id] = std::vector<float>(4, 1.5259e-05);
        }

        void onMousePress(int key, const KeyCallback& callback){
            mousePressCallbacks[key].push_back(callback);
        }
        void onMouseRelease(int key, const KeyCallback& callback){
            mouseReleaseCallbacks[key].push_back(callback);
        }
        
        void onCursorPositionChange(const CursorCallback& callback){
            cursorCallbacks.push_back(callback);
        }
        
        
        void onKeyPress(int key, const KeyCallback &callback){
            keyPressCallbacks[key].push_back(callback);
        }

        void onKeyRelease(int key, const KeyCallback &callback){
            keyReleaseCallbacks[key].push_back(callback);
        }
        
        void onJoystickButtonPress(int button, const JoystickCallback& callback){
            joystickPressCallbacks[button].push_back(callback);
        }

        void onJoystickButtonRelease(int button, const JoystickCallback& callback){
            joystickReleaseCallbacks[button].push_back(callback);
        }

        void onJoystickAnalogMove(int id, const AnalogCallback& callback){
            analogCallbacks[id].push_back(callback);
        }

        
    private:
        
        std::vector<unsigned int> joysticks;
        std::vector<CursorCallback> cursorCallbacks;
        std::unordered_map<int, std::vector<KeyCallback>> mousePressCallbacks, mouseReleaseCallbacks;
        std::unordered_map<int, std::vector<KeyCallback>> keyPressCallbacks, keyReleaseCallbacks;
        std::unordered_map<int, std::vector<std::function<void()>>> joystickPressCallbacks, joystickReleaseCallbacks;
        std::unordered_map<int, std::vector<unsigned char>> previousButtonState;
        std::unordered_map<int, std::vector<float>> previousAxesState;
        std::unordered_map<int, std::vector<AnalogCallback>> analogCallbacks;
        
        friend class Window;
    };

    class Window{
        // Position of the window
        struct Position{
            int x, y;
            Position(int x=50, int y=50): x(x), y(y){}
        } position;
        

        // Size of the window
        struct Size{
            int width, height;
            Size(int width=600, int height=600): width(width), height(height){}
        } size;


        void setDeltaTime(float t){
            deltaTime = t;
        }
        void setupCallbacks();
        // Initializes GLFW and create the window. setup callbacks, and execute initialization behaviors
        void init();

        static void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void cursorCallback(GLFWwindow* window, double xpos, double ypos);
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    public:
        typedef std::function<void()> Behavior;
        Window();
        ~Window();
        
        float getDeltaTime() const {
            return deltaTime;
        }
        void setFullScreen(bool yes);



        void addFrameBehavior(const Behavior& behavior){
            frameBehaviors.push_back(behavior);
        }
        void addInitBehavior(const Behavior& behavior){
            initBehaviors.push_back(behavior);
        }


        GLFWwindow * getHandle() const {
            return window;
        }
        void show();
        void close();


        inline void resize(const edw::Window::Size& size){
            if(isFullScreen()) setFullScreen(false);
            glfwSetWindowSize(window, size.width, size.height);
            this->size = size;
        }
        
        inline void move(const edw::Window::Position& position){
            glfwSetWindowPos(window, position.x, position.y);
            this->position = position;
        }
        
        inline Window::Position getPosition() const {
            return position;
        }

        void setTitleBarHidden(bool value=true){
            bool isDecorated = glfwGetWindowAttrib(window, GLFW_DECORATED);
            glfwSetWindowAttrib(window, GLFW_DECORATED, value && isDecorated ? GL_FALSE : GL_TRUE);
        }
        void setTitleBarShown(bool value=true){
            setTitleBarHidden(!value);
        }

        bool isFullScreen() const {
            return fullScreen;
        }

        Input input;
    private:
        std::vector<Behavior> initBehaviors, frameBehaviors;
        GLFWwindow * window;

        float deltaTime{};
        bool fullScreen = false;
    };
}
#endif // EDW_WINDOW_H