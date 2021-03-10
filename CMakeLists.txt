cmake_minimum_required(VERSION 3.14.5)

# Varibales setting
set(MY_PROJECT "NexusDemo")
set(MY_LIBRARY "Nexus")

set(NEXUS_INCLUDE_DIRS
	"${CMAKE_CURRENT_SOURCE_DIR}/Header"
	"${CMAKE_CURRENT_SOURCE_DIR}/External/stb_image")

set(NEXUS_SOURCE
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/Application.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/MessageLogger.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/Shader.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/Camera.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/External/stb_image/stb_image.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/External/stb_image/stb_image_write.cpp")

project (${MY_PROJECT})

# Library setting (Include cpp files and header files)
add_library(${MY_LIBRARY} ${NEXUS_SOURCE})
target_include_directories(${MY_LIBRARY} PUBLIC ${NEXUS_INCLUDE_DIRS})

# Loading GLFW (vcpkg)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${MY_LIBRARY} PRIVATE glfw)

# Loading GLM (vcpkg)
find_package(glm CONFIG REQUIRED)
target_link_libraries(${MY_LIBRARY} PRIVATE glm)

# Loading GLAD (vcpkg)
find_package(glad CONFIG REQUIRED)
target_link_libraries(${MY_LIBRARY} PRIVATE glad::glad)

# Loading ImGUI (vcpkg)
find_package(imgui CONFIG REQUIRED)
target_link_libraries(${MY_LIBRARY} PRIVATE imgui::imgui)

# Loading Assimp (vcpkg)
find_package(Assimp CONFIG REQUIRED)
target_link_libraries(${MY_LIBRARY} PRIVATE assimp::assimp)

# Excecutable file setting
add_executable(${MY_PROJECT} Source/Main.cpp)
target_link_libraries(${MY_PROJECT} PRIVATE glfw glm glad::glad imgui::imgui assimp::assimp)
target_link_libraries(${MY_PROJECT} PUBLIC ${MY_LIBRARY})