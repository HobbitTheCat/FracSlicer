cmake_minimum_required(VERSION 3.24)
project(IFS_Slicer LANGUAGES CXX CUDA)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -O3 --compiler-options '-fPIC'")

option(ENABLE_DEBUG_FEATURES "Enable custom debug code parts" OFF)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Armadillo REQUIRED)
find_package(TBB REQUIRED)
find_package(BLAS REQUIRED)
find_package(CUDAToolkit REQUIRED)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
list(FILTER SOURCE_FILES EXCLUDE REGEX "src/app_.*\\.cpp")
file(GLOB_RECURSE CUDA_FILES "src/*.cu")
file(GLOB_RECURSE VENDOR_FILES "vendor/*.cpp")

add_library(project_core STATIC ${SOURCE_FILES} ${CUDA_FILES} ${VENDOR_FILES})

target_include_directories(project_core 
    PUBLIC 
        include
        .
    PRIVATE 
        SYSTEM vendor
        vendor/imgui
        vendor/imguifiledialog
        vendor/nlohmann_json
)

target_compile_options(project_core PRIVATE -Wall -Wextra)

target_compile_options(project_core PUBLIC -ftemplate-depth=512) 
target_link_libraries(project_core PUBLIC TBB::tbb)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_link_options(project_core PUBLIC -ltbb)
endif()

set_source_files_properties(${VENDOR_FILES} PROPERTIES COMPILE_OPTIONS "-w")

if (ENABLE_DEBUG_FEATURES)
    target_compile_definitions(project_core PRIVATE MY_DEBUG_FLAG)
    target_compile_options(project_core PRIVATE -g)
endif()

target_link_libraries(project_core PUBLIC
    glfw
    GLEW::GLEW
    OpenGL::GL
    armadillo
    ${BLAS_LIBRARIES}
    TBB::tbb
    CUDA::cudart
    dl
)

file(GLOB APP_SOURCES "src/app_*.cpp")

foreach(APP_SRC ${APP_SOURCES})
    get_filename_component(APP_NAME ${APP_SRC} NAME_WE)
    add_executable(${APP_NAME} ${APP_SRC})
    target_link_libraries(${APP_NAME} PRIVATE project_core)
endforeach()

file(GLOB_RECURSE SHADER_FILES "*.glsl")
foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    configure_file(${SHADER} ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME} COPYONLY)
endforeach()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/res")
    add_custom_target(copy_res_folder ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/res"
                "${CMAKE_CURRENT_BINARY_DIR}/res"
        COMMENT "Copying resources to build directory..."
    )
endif()