cmake_minimum_required(VERSION 3.13)
project(refraction_openCL)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

add_executable(refraction_openCL main.cpp variables.cpp myMath.cpp gpuSetting.cpp vector.cpp gradient.cpp objectData.cpp lightPropergation.cpp lightPropergation.h casting.cpp casting.h)

INCLUDE_DIRECTORIES (/System/Library/Frameworks)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY})

target_include_directories (refraction_openCL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(refraction_openCL "/System/Library/Frameworks/GLUT.framework" "/System/Library/Frameworks/openGL.framework" ${OpenCL_LIBRARY} ${OpenCV_LIBS})