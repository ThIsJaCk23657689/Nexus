cmake_minimum_required(VERSION 3.14.5)

# Varibales setting
set(MY_PROJECT "NexusDemo")
set(MY_LIBRARY "Nexus")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(NEXUS_INCLUDE_DIRS
	"Header"
	"Header/Geometry/2D"
	"Header/Geometry/3D"
	"Header/Physics/"
	"Header/Physics/Collision"
	"Header/Physics/Collision/Collider"
	"External/imgui-transfer-function")

set(SOURCE_DIRS
	"Source/*.cpp"
	"External/imgui-transfer-function/*.cpp")

project (${MY_PROJECT} LANGUAGES CXX C)

file(GLOB_RECURSE NEXUS_SOURCE CONFIGURE_DEPENDS ${SOURCE_DIRS})

# Library setting (Include cpp files and header files)
add_library(${MY_LIBRARY} ${NEXUS_SOURCE})
find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
target_include_directories(${MY_LIBRARY} PUBLIC ${NEXUS_INCLUDE_DIRS} ${STB_INCLUDE_DIRS})

# Loading GLFW GLM GLAD ImGUI Assimp (vcpkg)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
# find_package(Assimp CONFIG REQUIRED)
target_link_libraries(${MY_LIBRARY} PUBLIC glfw glm::glm glad::glad imgui::imgui implot::implot)
# target_link_libraries(${MY_LIBRARY} PUBLIC glfw glm::glm glad::glad assimp::assimp)

add_custom_command(TARGET ${MY_LIBRARY} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/External/Nexus/Resource/ ${CMAKE_BINARY_DIR}/Resource/)