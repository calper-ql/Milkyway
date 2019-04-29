cmake_minimum_required(VERSION 3.14)
project(MilkywayVR)

set(CMAKE_CXX_STANDARD 17)

find_package(NGEN REQUIRED)
find_package(HiveEngine REQUIRED)

find_package(CUDA 10.1 REQUIRED)
message(STATUS ${CUDA_INCLUDE_DIRS})
include_directories(${CUDA_INCLUDE_DIRS})

find_package(glfw3 REQUIRED)
find_package( Threads )

include_directories("/usr/local/include/optix")

set(CUDA_NVCC_FLAGS "-arch sm_30 --keep-device-functions")

CUDA_COMPILE_PTX(planet ${CMAKE_CURRENT_SOURCE_DIR}/CUDA_PROGRAMS/planet.cu)
CUDA_COMPILE_PTX(anyhit ${CMAKE_CURRENT_SOURCE_DIR}/CUDA_PROGRAMS/anyhit.cu)
CUDA_COMPILE_PTX(raygen ${CMAKE_CURRENT_SOURCE_DIR}/CUDA_PROGRAMS/raygen.cu)
CUDA_COMPILE_PTX(exception ${CMAKE_CURRENT_SOURCE_DIR}/CUDA_PROGRAMS/exception.cu)
CUDA_COMPILE_PTX(miss ${CMAKE_CURRENT_SOURCE_DIR}/CUDA_PROGRAMS/miss.cu)
CUDA_COMPILE_PTX(triangle_attributes ${CMAKE_CURRENT_SOURCE_DIR}/CUDA_PROGRAMS/triangle_attributes.cu)
CUDA_COMPILE_PTX(boundingbox_triangle_indexed ${CMAKE_CURRENT_SOURCE_DIR}/CUDA_PROGRAMS/boundingbox_triangle_indexed.cu)
CUDA_COMPILE_PTX(intersection_triangle_indexed ${CMAKE_CURRENT_SOURCE_DIR}/CUDA_PROGRAMS/intersection_triangle_indexed.cu)
CUDA_COMPILE_PTX(atmosphere ${CMAKE_CURRENT_SOURCE_DIR}/CUDA_PROGRAMS/atmosphere.cu)
CUDA_COMPILE_PTX(atmosphere_anyhit ${CMAKE_CURRENT_SOURCE_DIR}/CUDA_PROGRAMS/atmosphere_anyhit.cu)

add_executable(MilkywayVR main.cpp OptixApp.h OptixApp.cc GL/glcorearb.h GL/gl3w.c GL/gl3w.h
        Utilities.h Utilities.cpp TextureRenderer.cpp TextureRenderer.h RTCamera.cpp RTCamera.h
        Planet.cc Planet.h DSLODOptixRenderer.cc DSLODOptixRenderer.h imgui/imconfig.h
        imgui/imgui.cpp
        imgui/imgui.h
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_internal.h
        imgui/imgui_widgets.cpp
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
        imgui/imgui_impl_glfw.h
        imgui/imgui_impl_glfw.cpp
        imgui/imgui_impl_opengl3.h
        imgui/imgui_impl_opengl3.cpp
        ${anyhit} ${planet} ${raygen} ${exception} ${miss} ${triangle_attributes} ${atmosphere} ${atmosphere_anyhit}
        ${boundingbox_triangle_indexed} ${intersection_triangle_indexed} Atmosphere.cpp Atmosphere.h CUDA_PROGRAMS/util.h)

target_compile_features(MilkywayVR PUBLIC cxx_std_17)
set(CMAKE_REQUIRED_FLAGS -std=c++17)

link_directories("/usr/local/lib64")
find_library(OPTIX_LIBRARIES optix optixu "/usr/local/lib64")
target_link_libraries(MilkywayVR ${HiveEngine_LIBRARIES} ${NGEN_LIBRARIES} glfw dl ${OPTIX_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} stdc++fs)
configure_file(test_graph.json ${CMAKE_CURRENT_BINARY_DIR}/test_graph.json COPYONLY)