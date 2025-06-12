cmake_minimum_required(VERSION 3.16)
project(IWannaBeTheBoshy)

set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0135 NEW)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

add_compile_definitions(RESOURCE_DIR="${CMAKE_SOURCE_DIR}/Resources")

FetchContent_Declare(
        ptsd
        URL https://github.com/ntut-open-source-club/practical-tools-for-simple-design/archive/refs/tags/v0.3-rc1.zip
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/PTSD
)

FetchContent_MakeAvailable(ptsd)

include(files.cmake)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

list(TRANSFORM SRC_FILES PREPEND ${SRC_DIR}/)
list(TRANSFORM INCLUDE_FILES PREPEND ${INCLUDE_DIR}/)

add_executable(${PROJECT_NAME}
        ${SRC_DIR}/main.cpp
        ${SRC_FILES}
        ${HEADER_FILES}

        # App and core logic
        src/AppStart.cpp
        src/AppUpdate.cpp

        # Map & Resources
        src/MapInfoLoader.cpp
        src/ResourceManager.cpp
        src/CheckPoint.cpp
        src/imgui.cpp

        # Game Objects
        src/Character.cpp
        src/AnimatedCharacter.cpp
        src/Bullet.cpp
        src/bear.cpp
        src/phase2trap.cpp
        src/Bird.cpp
        src/BgmManager.cpp
        src/Boss1/Boss1.cpp
        src/Boss1/BulletTypeA.cpp
        src/Boss1/BulletTypeB.cpp
        src/Boss1/BulletTypeC.cpp
        src/Boss1/LightAttack.cpp
        src/Boss1/BossDeadAnimation.cpp
        src/spider.cpp

        # Headers
        include/BackgroundImage.hpp
        include/ResourceManager.hpp
        include/Character.hpp
        include/AnimatedCharacter.hpp
        include/Menu.hpp
        include/Bullet.hpp
        include/World.hpp
        include/GameObjectHelper.hpp
        include/MapInfoLoader.hpp
        include/MapObject/CheckPoint.hpp
        include/MapObject/fallingground.hpp
        include/MapObject/phase2trap.hpp
        include/MapObject/bird.hpp
        include/MapObject/spider.hpp
        include/bear.hpp
        include/BgmManager.hpp
        include/Boss1/Boss1.hpp
        include/Boss1/BulletTypeA.hpp
        include/Boss1/BulletTypeB.hpp
        include/Boss1/BulletTypeC.hpp
        include/Boss1/LightAttack.hpp
        include/Boss1/BossHpInfo.hpp
        include/Boss1/BossDeadAnimation.hpp
        src/Menu.cpp
        include/AppMembers.hpp
        include/GameOverUI.hpp
        src/GameOverUI.cpp
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Resources")
else()
    message(FATAL_ERROR "Relative RESOURCE_DIR is WIP. Please use `-DCMAKE_BUILD_TYPE=Debug` for now.")
endif()

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${DEPENDENCY_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/PTSD/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
        SDL2::SDL2main
        PTSD
)
