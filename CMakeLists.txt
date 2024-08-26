cmake_minimum_required(VERSION 3.20)

project(edw VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)



# Add the library
add_library(edw STATIC src/Window.cpp)


# Specify the include directories
target_include_directories(edw PUBLIC include)


# Find dependencies
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)


# Link dependencies
target_link_libraries(edw PUBLIC GLEW::GLEW glfw)


# Install targets
install(TARGETS edw EXPORT edwTargets 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)


# Install headers
install(DIRECTORY include/ DESTINATION include)



# Install CMakePackageConfigHelpers
include(CMakePackageConfigHelpers)



# Generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/edwConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY AnyNewerVersion
    
)




# Create the config file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/edwConfig.cmake"
    INSTALL_DESTINATION lib/cmake/edw
)



# Install the config files
install(FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/edwConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/edwConfigVersion.cmake"
    DESTINATION lib/cmake/edw
)


set_target_properties(edw PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "")

# Export the targets to a script
install(EXPORT edwTargets 
    FILE edwTargets.cmake 
    NAMESPACE edw:: 
    DESTINATION "lib/cmake/edw"
)