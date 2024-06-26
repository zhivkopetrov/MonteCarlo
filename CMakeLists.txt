cmake_minimum_required(VERSION 3.5.1)

project(batman_integration)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake_helpers/helpers.cmake)
set(CMAKE_MODULE_PATH 
    ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake_helpers/find_modules)

if(UNIX)
    set(SDL_IMAGE_PKG_NAME SDL2_image)
    set(SDL_TTF_PKG_NAME SDL2_ttf)
elseif(WIN32)
    set(SDL_IMAGE_PKG_NAME SDL2_IMAGE)
    set(SDL_TTF_PKG_NAME SDL2_TTF)
else()
    message("Error: Unsupported OS")
endif()

find_package(SDL2 REQUIRED)
find_package(${SDL_IMAGE_PKG_NAME} REQUIRED)
find_package(${SDL_TTF_PKG_NAME} REQUIRED)
        
set(_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}) 
         
#file(GLOB...) allows for wildcard additions:
file(GLOB _SOURCES 
        ${_BASE_DIR}/*.cpp
        ${_BASE_DIR}/sdl/*.cpp
        ${_BASE_DIR}/gameentities/*.cpp
        ${_BASE_DIR}/pathfinding/*.cpp
        ${_BASE_DIR}/common/*.cpp
)
                  
add_executable(${PROJECT_NAME}
               ${_SOURCES})
                     
set_target_cpp_standard(${PROJECT_NAME} 17)
enable_target_warnings(${PROJECT_NAME})

target_include_directories(
    ${PROJECT_NAME}
        PRIVATE
            ${_BASE_DIR}
            ${SDL2_INCLUDE_DIR}
            ${SDL2_IMAGE_INCLUDE_DIR}
            ${SDL2_TTF_INCLUDE_DIR}
)

target_link_libraries(
    ${PROJECT_NAME}
        PRIVATE
            ${SDL2_LIBRARY}       # -lSDL2 flag
            ${SDL2_IMAGE_LIBRARY} # -lSDL2_image flag
            ${SDL2_TTF_LIBRARY}   # -lSDL2_ttf flag
            m                     # -libm flag
)

if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
    target_link_libraries(
        ${PROJECT_NAME}
            PRIVATE
                stdc++            # -libstdc++ flag
    )
endif()