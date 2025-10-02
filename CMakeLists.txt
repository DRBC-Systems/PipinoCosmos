cmake_minimum_required(VERSION 3.19)
project(PipinoCosmos)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# -------------------------------
# Find Qt6 Widgets
# -------------------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# -------------------------------
# Copy assets
# -------------------------------
file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR})

# -------------------------------
# UI files (just list them, AUTOUIC handles them)
# -------------------------------
set(UI_FILES
    ${PROJECT_SOURCE_DIR}/Externals/Qt/MainWindow.ui
)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${PROJECT_SOURCE_DIR}/Externals/Qt
)

# -------------------------------
# Source and header files
# -------------------------------
file(GLOB_RECURSE CONTROLLER_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/Controller/src/*.cpp ${PROJECT_SOURCE_DIR}/Controller/include/*.h)
file(GLOB_RECURSE MODEL_SOURCES      CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/Model/src/*.cpp ${PROJECT_SOURCE_DIR}/Model/include/*.h)
file(GLOB_RECURSE VIEW_SOURCES       CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/View/src/*.cpp ${PROJECT_SOURCE_DIR}/View/include/*.h)

set(SOURCES
    ${CONTROLLER_SOURCES}
    ${MODEL_SOURCES}
    ${VIEW_SOURCES}
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${UI_FILES}
)

# -------------------------------
# Executable
# -------------------------------
add_executable(${PROJECT_NAME} ${SOURCES})

# -------------------------------
# Include dirs
# -------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/Controller/include
    ${PROJECT_SOURCE_DIR}/Model/include
    ${PROJECT_SOURCE_DIR}/View/include
)

# -------------------------------
# Link Qt6 Widgets
# -------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)