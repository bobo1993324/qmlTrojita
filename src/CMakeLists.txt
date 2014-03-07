project(qmltrojita-src)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++0x -g")

include_directories(
    plugin
    trojita/src
)


add_executable(qmltrojita main.cpp qmltrojita.cpp mailBackend.cpp ${QML_SRCS})

qt5_use_modules(qmltrojita Widgets Quick)

file(GLOB_RECURSE QML_SRCS *.qml *.js)
add_custom_target(reminders-qmlfiles ALL
    COMMAND cp -rf ${CMAKE_CURRENT_SOURCE_DIR}/qml ${CMAKE_CURRENT_BINARY_DIR}
	    DEPENDS ${QMLFILES}
	)

target_link_libraries(qmltrojita email-plugin trojita)

add_subdirectory(plugin)
add_subdirectory(trojita)
