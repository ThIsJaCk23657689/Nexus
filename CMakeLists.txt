cmake_minimum_required(VERSION 3.14.5)

# Varibales setting
set(MY_PROJECT "NexusDemo")
set(MY_LIBRARY "Nexus")

set(NEXUS_INCLUDE_DIRS
	"Header"
	"Header/Geometry/2D"
	"Header/Geometry/3D"
	"External/stb_image"
	"External/imgui"
	"External/implot"
	"External/imgui-transfer-function")

set(SOURCE_DIRS
	"Source/*.cpp"
	"External/stb_image/*.cpp"
	"External/imgui/*.cpp"
	"External/implot/*.cpp"
	"External/imgui-transfer-function/*.cpp")

project (${MY_PROJECT} LANGUAGES CXX C)

file(GLOB_RECURSE NEXUS_SOURCE CONFIGURE_DEPENDS ${SOURCE_DIRS})

# Library setting (Include cpp files and header files)
add_library(${MY_LIBRARY} ${NEXUS_SOURCE})
target_include_directories(${MY_LIBRARY} PUBLIC ${NEXUS_INCLUDE_DIRS})

# Loading GLFW GLM GLAD ImGUI Assimp (vcpkg)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Assimp CONFIG REQUIRED)
target_link_libraries(${MY_LIBRARY} PUBLIC glfw glm::glm glad::glad assimp::assimp)