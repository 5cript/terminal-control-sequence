cmake_minimum_required(VERSION 3.5)

project(terminal-control-sequence LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
	${PROJECT_NAME}
	main.cpp
	input_to_control.cpp
	test_keys.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC -lpthread)
