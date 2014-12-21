cmake_minimum_required(VERSION 2.8)

project(sample_0)

set(cpps src/main.cpp src/shader.cpp src/hw1.cpp src/texture_loader.cpp
         src/tiny_obj_loader.cc)
set(headers src/shader.h src/common.h src/callbacks.h src/hw1.h
            src/filenames.h src/texture_loader.h src/tiny_obj_loader.h
            src/ubuntu_fix.h)

IF (WIN32)
   set(EXTERNAL_LIBS ${PROJECT_SOURCE_DIR}/../../ext CACHE STRING "external libraries location")

   include_directories(${EXTERNAL_LIBS}/AntTweakBar/include)
   include_directories(${EXTERNAL_LIBS}/freeglut/include)
   include_directories(${EXTERNAL_LIBS}/glew-1.10.0/include)
   include_directories(${EXTERNAL_LIBS}/glm)

   link_directories(${EXTERNAL_LIBS}/AntTweakBar/lib)
   link_directories(${EXTERNAL_LIBS}/freeglut/lib)
   link_directories(${EXTERNAL_LIBS}/glew-1.10.0/lib/Release/Win32)

   add_executable(main ${cpps} ${headers})

   target_link_libraries(main freeglut glew32 AntTweakBar)

   add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${EXTERNAL_LIBS}/AntTweakBar/lib"
        $<TARGET_FILE_DIR:main>)
   add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${EXTERNAL_LIBS}/glew-1.10.0/bin/Release/Win32"
        $<TARGET_FILE_DIR:main>)
   add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${EXTERNAL_LIBS}/freeglut/bin"
        $<TARGET_FILE_DIR:main>)

   add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/shaders"
        ${PROJECT_BINARY_DIR}/shaders)
   add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/shaders"
        $<TARGET_FILE_DIR:main>/shaders)
ELSE (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -pedantic")

    add_executable(main ${cpps} ${headers})


    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)

    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_directories(${GLEW_LIBRARY_DIRS})
    add_definitions(${GLEW_DEFINITIONS})
    if(NOT GLEW_FOUND)
       message(Error "GLEW not found")
    endif(NOT GLEW_FOUND)

   include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
   target_link_libraries(main AntTweakBar X11 GL glut GLEW freeimage pthread)
ENDIF (WIN32)
