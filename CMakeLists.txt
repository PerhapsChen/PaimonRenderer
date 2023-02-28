cmake_minimum_required(VERSION 3.15)
project(RenderSomething VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

# add imgui as static lib
set(imgui_SOURCE_DIR_ ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/imgui)
file(GLOB imgui_sources CONFIGURE_DEPENDS  "${imgui_SOURCE_DIR_}/*.cpp")
add_library(imgui ${imgui_sources} )
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR_} )

# build assimp
add_subdirectory(3rdParty/assimp)

# set our source file and imgui backends source file
file(GLOB imgui_impl CONFIGURE_DEPENDS  
"${imgui_SOURCE_DIR_}/backends/imgui_impl_glfw.cpp" 
"${imgui_SOURCE_DIR_}/backends/imgui_impl_opengl3.cpp")
FILE(GLOB SRC_LIST CONFIGURE_DEPENDS "src/*.cpp")
# build our executable
add_executable(RenderSomething ${SRC_LIST} ${imgui_impl})
target_include_directories(RenderSomething PUBLIC
                ${imgui_SOURCE_DIR_}/backends
                ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glfw/include/ 
                ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glm/include/
                ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/stb/include/
                ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glad/include/)
target_link_directories(RenderSomething PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glfw/lib/
                                         ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/glad/lib/)
target_link_libraries(RenderSomething assimp imgui glfw3.lib glad.lib opengl32.lib gdi32.lib shell32.lib)
