cmake_minimum_required(VERSION 3.19)

# Set policies for better cross-platform compatibility
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)  # Use <PackageName>_ROOT variables
endif()
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)  # option() honors normal variables
endif()

project(PipinoCosmos VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure we use the same C++ standard across all targets
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Qt's automatic tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# -------------------------------
# Find Qt6 Widgets
# -------------------------------
# Try to find Qt6 automatically first
find_package(Qt6 REQUIRED COMPONENTS Core Network Widgets)


# If not found, try common installation paths
if(NOT Qt6_FOUND)
    message(STATUS "Qt6 not found in standard paths, trying common installation locations...")
    
    # Define possible Qt6 paths for different platforms
    set(QT6_POSSIBLE_PATHS)
    
    if(WIN32)
        # Windows paths (Qt installer, vcpkg, etc.)
        list(APPEND QT6_POSSIBLE_PATHS
            "C:/Qt/6.9.3/msvc2022_64/lib/cmake/Qt6"
            "C:/Qt/6.7.3/msvc2022_64/lib/cmake/Qt6"
            "C:/Qt/6.6.3/msvc2022_64/lib/cmake/Qt6"
            "C:/Qt/6.5.3/msvc2022_64/lib/cmake/Qt6"
            "C:/vcpkg/installed/x64-windows/share/qt6"
            "$ENV{QTDIR}/lib/cmake/Qt6"
        )
    elseif(APPLE)
        # macOS paths (Qt installer, Homebrew, MacPorts)
        list(APPEND QT6_POSSIBLE_PATHS
            "$ENV{HOME}/Qt/6.9.3/macos/lib/cmake/Qt6"
            "$ENV{HOME}/Qt/6.7.3/clang_64/lib/cmake/Qt6"
            "$ENV{HOME}/Qt/6.6.3/clang_64/lib/cmake/Qt6"
            "$ENV{HOME}/Qt/6.5.3/clang_64/lib/cmake/Qt6"
            "/opt/homebrew/lib/cmake/Qt6"
            "/usr/local/lib/cmake/Qt6"
            "/opt/local/libexec/qt6/lib/cmake/Qt6"
            "$ENV{QTDIR}/lib/cmake/Qt6"
        )
    else()
        # Linux paths (Qt installer, system packages, custom builds)
        list(APPEND QT6_POSSIBLE_PATHS
            "$ENV{HOME}/Qt/6.9.3/gcc_64/lib/cmake/Qt6"
            "$ENV{HOME}/Qt/6.7.3/gcc_64/lib/cmake/Qt6"
            "$ENV{HOME}/Qt/6.6.3/gcc_64/lib/cmake/Qt6"
            "$ENV{HOME}/Qt/6.5.3/gcc_64/lib/cmake/Qt6"
            "/usr/lib/x86_64-linux-gnu/cmake/Qt6"
            "/usr/lib64/cmake/Qt6"
            "/usr/local/lib/cmake/Qt6"
            "$ENV{QTDIR}/lib/cmake/Qt6"
        )
    endif()
    
    # Try each possible path
    foreach(QT_PATH ${QT6_POSSIBLE_PATHS})
        if(EXISTS "${QT_PATH}")
            set(Qt6_DIR "${QT_PATH}")
            message(STATUS "Found Qt6 at: ${QT_PATH}")
            break()
        endif()
    endforeach()
    
    # Try to find Qt6 again with the discovered path
    if(Qt6_DIR)
        find_package(Qt6 QUIET COMPONENTS Widgets)
    endif()
endif()

# Final check - require Qt6 or fail with helpful message
if(NOT Qt6_FOUND)
    message(FATAL_ERROR 
        "Qt6 not found! Please install Qt6 or set Qt6_DIR to your Qt6 installation.\n"
        "Examples:\n"
        "  Windows: cmake -DQt6_DIR=C:/Qt/6.8.0/msvc2022_64/lib/cmake/Qt6 ..\n"
        "  macOS:   cmake -DQt6_DIR=$HOME/Qt/6.8.0/clang_64/lib/cmake/Qt6 ..\n"
        "  Linux:   cmake -DQt6_DIR=$HOME/Qt/6.8.0/gcc_64/lib/cmake/Qt6 ..\n"
        "Or install Qt6 via package manager:\n"
        "  Ubuntu/Debian: sudo apt install qt6-base-dev\n"
        "  macOS: brew install qt@6\n"
        "  Windows: winget install Qt.Qt.6"
    )
endif()

message(STATUS "Using Qt6 from: ${Qt6_DIR}")


# -------------------------------
# Copy assets and external resources
# -------------------------------
file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR})

# Copy bootstrap API data files for runtime access
file(COPY Externals/2025-bootstrap-main/api/data 
     DESTINATION ${CMAKE_BINARY_DIR}/api)

# -------------------------------
# UI files (just list them, AUTOUIC handles them)
# -------------------------------
set(UI_FILES
    ${PROJECT_SOURCE_DIR}/Externals/Qt/MainWindow.ui
    ${PROJECT_SOURCE_DIR}/Externals/Qt/MultipleChoiceWindow.ui
    ${PROJECT_SOURCE_DIR}/Externals/Qt/SettingsWindow.ui
    ${PROJECT_SOURCE_DIR}/Externals/Qt/ScanWindow.ui
    ${PROJECT_SOURCE_DIR}/Externals/Qt/TheoryWindow.ui
)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${PROJECT_SOURCE_DIR}/Externals/Qt
)

# -------------------------------
# Source and header files
# -------------------------------
# Collect source files
file(GLOB_RECURSE CONTROLLER_SOURCES CONFIGURE_DEPENDS 
    "${PROJECT_SOURCE_DIR}/Controller/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/Controller/include/*.h"
)
file(GLOB_RECURSE MODEL_SOURCES CONFIGURE_DEPENDS 
    "${PROJECT_SOURCE_DIR}/Model/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/Model/include/*.h"
)
file(GLOB_RECURSE VIEW_SOURCES CONFIGURE_DEPENDS 
    "${PROJECT_SOURCE_DIR}/View/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/View/include/*.h"
)
file(GLOB_RECURSE AI_SOURCES CONFIGURE_DEPENDS 
    "${PROJECT_SOURCE_DIR}/AI/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/AI/include/*.h"
)

# Main source files
set(MAIN_SOURCES
    "${PROJECT_SOURCE_DIR}/main.cpp"
)

# Combine all sources
set(SOURCES
    ${CONTROLLER_SOURCES}
    ${MODEL_SOURCES}
    ${VIEW_SOURCES}
    ${AI_SOURCES}
    ${MAIN_SOURCES}
    ${UI_FILES}
)

# Print source files for debugging
message(STATUS "Controller sources: ${CONTROLLER_SOURCES}")
message(STATUS "Model sources: ${MODEL_SOURCES}")
message(STATUS "View sources: ${VIEW_SOURCES}")
message(STATUS "AI sources: ${AI_SOURCES}")
message(STATUS "UI files: ${UI_FILES}")

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
    ${PROJECT_SOURCE_DIR}/AI/include
    ${CMAKE_CURRENT_BINARY_DIR}  # For generated UI headers (ui_*.h)
)

# -------------------------------
# Link Qt6 Widgets
# -------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Network Qt6::Widgets)


# -------------------------------
# Platform-specific settings
# -------------------------------
if(WIN32)
    # Windows-specific settings
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # Copy Qt DLLs on Windows (for development)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Note: You may need to add Qt6 DLLs to PATH or copy them to the output directory"
        )
    endif()
elseif(APPLE)
    # macOS-specific settings
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    )
endif()

# -------------------------------
# Installation (optional)
# -------------------------------
# Set a reasonable default install prefix for Windows
if(WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install path prefix" FORCE)
endif()

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

# Install assets
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Assets
    DESTINATION bin
    OPTIONAL
)

# Install bootstrap API data
install(DIRECTORY ${CMAKE_SOURCE_DIR}/Externals/2025-bootstrap-main/api/data
    DESTINATION bin/api
    OPTIONAL
)

# -------------------------------
# External Components (Bootstrap API)
# -------------------------------
# Check if Node.js is available for the bootstrap API
find_program(NODE_EXECUTABLE node)
find_program(NPM_EXECUTABLE npm)

if(NODE_EXECUTABLE AND NPM_EXECUTABLE)
    message(STATUS "Node.js found: ${NODE_EXECUTABLE}")
    message(STATUS "npm found: ${NPM_EXECUTABLE}")
    
    # Add custom target to build the bootstrap API
    add_custom_target(bootstrap-api
        COMMENT "Building Bootstrap API (NestJS)"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Externals/2025-bootstrap-main/api
        COMMAND ${NPM_EXECUTABLE} install
        COMMAND ${NPM_EXECUTABLE} run build
        VERBATIM
    )
    
    # Add custom target to run the bootstrap API in development mode
    add_custom_target(run-bootstrap-api
        COMMENT "Running Bootstrap API in development mode"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Externals/2025-bootstrap-main/api
        COMMAND ${NPM_EXECUTABLE} run start:dev
        VERBATIM
    )
    
    message(STATUS "Bootstrap API targets available:")
    message(STATUS "  - 'make bootstrap-api' or 'cmake --build . --target bootstrap-api' to build")
    message(STATUS "  - 'make run-bootstrap-api' or 'cmake --build . --target run-bootstrap-api' to run")
else()
    message(STATUS "Node.js/npm not found. Bootstrap API targets will not be available.")
    message(STATUS "To use the Bootstrap API, install Node.js from https://nodejs.org/")
endif()

# -------------------------------
# Summary
# -------------------------------
message(STATUS "")
message(STATUS "=== PipinoCosmos Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt6 location: ${Qt6_DIR}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "========================================")
message(STATUS "")