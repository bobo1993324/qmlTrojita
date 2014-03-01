project(ubuntu-emailclient-app)
cmake_minimum_required(VERSION 2.8.9)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Widgets)
find_package(Qt5Quick)
find_package(Qt5Network)

add_subdirectory(src)
