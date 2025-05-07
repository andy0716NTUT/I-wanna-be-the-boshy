cmake_minimum_required(VERSION 3.16)
project(REPLACE_WITH_YOUR_PROJECT_NAME)

set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0135 NEW)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
add_compile_definitions(RESOURCE_DIR="${CMAKE_SOURCE_DIR}/Resources")
FetchContent_Declare(
        ptsd

        URL         https://github.com/ntut-open-source-club/practical-tools-for-simple-design/archive/refs/tags/v0.3-rc1.zip

        # Or alternatively if you want to use a specific git commit instead of a release:
        # GIT_REPOSITORY  https://github.com/ntut-open-source-club/practical-tools-for-simple-design.git
        # GIT_TAG         <git_tag>

        SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/PTSD
)

FetchContent_MakeAvailable(ptsd)

include(files.cmake)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
list(TRANSFORM SRC_FILES PREPEND ${SRC_DIR}/)
list(TRANSFORM INCLUDE_FILES PREPEND ${INCLUDE_DIR}/)

# file(GLOB_RECURSE SRC_FILES src/*)
# file(GLOB_RECURSE HEADER_FILES include/*)

add_executable(${PROJECT_NAME}
        ${SRC_DIR}/main.cpp
        ${SRC_FILES}
        ${HEADER_FILES}
        src/AppStart.cpp
        src/AppUpdate.cpp
        include/BackgroundImage.hpp
        src/ResourceManager.cpp
        include/ResourceManager.hpp
        src/Character.cpp
        include/Character.hpp
        include/Bullet.hpp
        include/AnimatedCharacter.hpp
        src/AnimatedCharacter.cpp
        include/Menu.hpp
        src/Menu.cpp
        src/MapInfoLoader.cpp
        include/MapInfoLoader.hpp
        src/Bullet.cpp
        include/World.hpp
        include/MapObject/CheckPoint.hpp
        src/CheckPoint.cpp
        src/imgui.cpp
        include/MapObject/fallingground.hpp
        include/GameObjectHelper.hpp
        include/enemy.hpp
        src/enemy.cpp
        include/enemy.hpp
        include/MapObject/phase2trap.hpp
        src/phase2trap.cpp
        include/MapObject/bird.hpp
        src/Bird.cpp
        src/BgmManager.cpp
        include/BgmManager.hpp
        include/Boss1/Boss1.hpp
        src/Boss1/Boss1.cpp
        include/Boss1/BulletTypeA.hpp
        src/Boss1/BulletTypeA.cpp
        include/Boss1/LightAttack.hpp
        src/Boss1/LightAttack.cpp
        include/Boss1/BulletTypeB.hpp
        src/Boss1/BulletTypeB.cpp
        include/Boss1/BulletTypeC.hpp
        src/Boss1/BulletTypeC.cpp
        include/Boss1/BossHpInfo.hpp
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Resources")
else()
    message(FATAL_ERROR "relative RESOURCE_DIR is WIP, Please use `-DCMAKE_BUILD_TYPE=Debug` build for now.")
    # target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCE_DIR="${CMAKE_INSTALL_PREFIX}/Resources")
endif()

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${DEPENDENCY_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/PTSD/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}
        SDL2::SDL2main
        PTSD
)