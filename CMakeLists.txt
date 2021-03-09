cmake_minimum_required (VERSION 3.8)

# Variable setting
set(TARGET_NAME "Nexus")

set(NEXUS_INCLUDE_DIRS
	"${CMAKE_CURRENT_SOURCE_DIR}/Header"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source"
	"${CMAKE_CURRENT_SOURCE_DIR}/External/stb_image")

set(NEXUS_SOURCE
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/Application.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/Shader.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/Camera.cpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/External/stb_image/stb_image.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/External/stb_image/stb_image_write.cpp")

# Project setting
project (${TARGET_NAME})
# add_library(${TARGET_NAME}　${NEXUS_SOURCE})
add_executable(${TARGET_NAME} Source/Main.cpp)

# Loading GLFW
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE glfw)

# Loading GLM
find_package(glm CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE glm)

# Loading GLAD
find_package(glad CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE glad::glad)

# Loading ImGUI
find_package(imgui CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE imgui::imgui)

# Loading Assimp
find_package(Assimp CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE assimp::assimp)

target_include_directories(${TARGET_NAME} PUBLIC ${NEXUS_INCLUDE_DIRS})

# target_link_libraries(${TARGET_NAME} PUBLIC ${NEXUS_SOURCE})