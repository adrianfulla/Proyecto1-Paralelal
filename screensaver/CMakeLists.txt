#cmake_minimum_required(VERSION 3.29)
#project(screensaver)
#
#set(CMAKE_CXX_STANDARD 20)
#
#add_executable(screensaver main.cpp)
cmake_minimum_required(VERSION 3.15)
project(OpenGL_Screensaver)

set(CMAKE_CXX_STANDARD 17)

# Find the GLFW package installed via Homebrew
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# Include directories
include_directories(${GLFW_INCLUDE_DIRS})

# Link the GLFW library
link_directories(${GLFW_LIBRARY_DIRS})

# Add the executable
add_executable(OpenGL_Screensaver main.cpp)

# Link necessary libraries
target_link_libraries(OpenGL_Screensaver ${GLFW_LIBRARIES} "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
