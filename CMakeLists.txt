cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME CodeAnimator)

project(${PROJECT_NAME})

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

#OpenGL
find_package(OpenGL REQUIRED)

find_package( PythonLibs 3.5 ) # - do not find PythonLibs (only PC tested)
if (NOT PYTHONLIBS_FOUND)
  message("NOT PYTHONLIBS_FOUND")
  find_package( PythonInterp 3.5 )
  if (PYTHONINTERP_FOUND)
    # get PYTHON_ROOT_DIR from PYTHON_EXECUTABLE
    string(FIND ${PYTHON_EXECUTABLE} "\\" LAST_SLASH REVERSE)
    if (${LAST_SLASH} LESS 0)
      string(FIND ${PYTHON_EXECUTABLE} "/" LAST_SLASH REVERSE)
    endif()
    string(SUBSTRING ${PYTHON_EXECUTABLE} 0 ${LAST_SLASH} PYTHON_ROOT_DIR)
    set(PYTHON_INCLUDE_DIR "${PYTHON_ROOT_DIR}/include")
    set(PYTHON_LIBRARY "${PYTHON_ROOT_DIR}/libs/python35${CMAKE_STATIC_LIBRARY_SUFFIX}")
  else()
    set(PYTHON_INCLUDE_DIR "c:/Python35/include")
    set(PYTHON_LIBRARY "c:/Python35/libs/python35${CMAKE_STATIC_LIBRARY_SUFFIX}")
  endif()
endif()

message("PYTHON_INCLUDE_DIR: " ${PYTHON_INCLUDE_DIR})
message("PYTHON_LIBRARY: " ${PYTHON_LIBRARY})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

file(GLOB_RECURSE SOURCES sources/*.cpp sources/*.h sources/*.c)

set(RESOURCE ${CMAKE_SOURCE_DIR}/sources/resources.qrc)
qt5_add_resources(RESOURCE_ADDED ${RESOURCE})

include_directories(
    ${PYTHON_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/sources/
    )

add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCE_ADDED})

qt5_use_modules(${PROJECT_NAME}
    Core
    Gui
    Widgets
    OpenGL)

target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARY} Qt5::Widgets Qt5::Gui Qt5::Core Qt5::OpenGL ${OPENGL_LIBRARIES})
