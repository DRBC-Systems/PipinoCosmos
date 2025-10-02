cmake_minimum_required(VERSION 3.19)
project(PipinoCosmos)

set(CMAKE_CXX_STANDARD 17)

# -------------------------------
# Set the path to your Qt6 installation (MSVC 64-bit)
# -------------------------------
set(Qt6_DIR "C:/Qt/6.9.3/msvc2022_64/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH ${Qt6_DIR})


# -------------------------------
# Find Qt6 Widgets
# -------------------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets)


# -------------------------------
# Copy assets
# -------------------------------
file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR})

# -------------------------------
# UI files
# -------------------------------
set(UI_FILES
    ${PROJECT_SOURCE_DIR}/Externals/Qt/MainWindow.ui
   # ${PROJECT_SOURCE_DIR}/Externals/Qt/exBox.ui
)

qt6_wrap_ui(UI_HEADERS ${UI_FILES})

# -------------------------------
# Include directories
# -------------------------------
include_directories(
    ${PROJECT_SOURCE_DIR}/Controller/include
    ${PROJECT_SOURCE_DIR}/Model/include
    ${PROJECT_SOURCE_DIR}/View/include
    ${CMAKE_CURRENT_BINARY_DIR}   # for generated ui_*.h
)

# -------------------------------
# Source files
# -------------------------------
file(GLOB_RECURSE CONTROLLER_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/Controller/src/*.cpp)
file(GLOB_RECURSE MODEL_SOURCES      CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/Model/src/*.cpp)
file(GLOB_RECURSE VIEW_SOURCES       CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/View/src/*.cpp)

set(SOURCES
    ${CONTROLLER_SOURCES}
    ${MODEL_SOURCES}
    ${VIEW_SOURCES}
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${UI_HEADERS}
)

# -------------------------------
# Executable
# -------------------------------
add_executable(${PROJECT_NAME} ${SOURCES})

# -------------------------------
# Link Qt6 Widgets
# -------------------------------
target_link_libraries(${PROJECT_NAME} Qt6::Widgets)