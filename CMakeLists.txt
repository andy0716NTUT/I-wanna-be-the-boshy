cmake_minimum_required(VERSION 3.24)
project(IWannaBeTheBoshy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BOSHY_BUILD_TESTS "Build lightweight headless regression tests" ON)
include(CTest)

cmake_policy(SET CMP0135 NEW)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
        ptsd
        URL https://github.com/ntut-open-source-club/practical-tools-for-simple-design/archive/refs/tags/v0.3-rc1.zip
        URL_HASH SHA256=600da369a106913d16f22ad73d3c9fd4d1c38cfdae05c7ac993f21d67b46360d
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/PTSD
)

FetchContent_MakeAvailable(ptsd)

# Override only the upstream absolute asset definition, keeping the pinned source.
get_target_property(PTSD_DEFINITIONS PTSD COMPILE_DEFINITIONS)
list(FILTER PTSD_DEFINITIONS EXCLUDE REGEX "^PTSD_ASSETS_DIR=")
set_property(TARGET PTSD PROPERTY COMPILE_DEFINITIONS "${PTSD_DEFINITIONS}")
target_compile_definitions(PTSD PRIVATE PTSD_ASSETS_DIR="PTSDAssets")

include(files.cmake)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

list(TRANSFORM SRC_FILES PREPEND ${SRC_DIR}/)
list(TRANSFORM INCLUDE_FILES PREPEND ${INCLUDE_DIR}/)

add_executable(${PROJECT_NAME}
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/RuntimePaths.cpp
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
        src/PlayerManager.cpp
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
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /utf-8)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -finput-charset=UTF-8)
endif()

option(BOSHY_BUILD_SCENE_PROBES "Build opt-in graphical baseline probes" OFF)
if(BOSHY_BUILD_SCENE_PROBES)
    include(cmake/SceneProbes.cmake)
endif()

option(BOSHY_BUILD_REGRESSION_PROBES "Build instrumented original/current regression executables" OFF)
if(BOSHY_BUILD_REGRESSION_PROBES)
    include(cmake/RegressionProbes.cmake)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCE_DIR="Resources")
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${DEPENDENCY_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/PTSD/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
        SDL2::SDL2main
        PTSD
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ptsd_SOURCE_DIR}/assets/shaders" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/PTSDAssets/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/config.json" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/config.json"
    COMMAND ${CMAKE_COMMAND}
        "-DEXECUTABLE=$<TARGET_FILE:${PROJECT_NAME}>"
        "-DCOMPILER=${CMAKE_CXX_COMPILER}" "-DOBJDUMP=${CMAKE_OBJDUMP}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/DeployRuntime.cmake"
    VERBATIM)

if(BOSHY_BUILD_TESTS AND BUILD_TESTING)
    # Upstream registers Tests_NOT_BUILT unless this excluded target is built.
    set_property(TARGET Tests PROPERTY EXCLUDE_FROM_ALL FALSE)
    add_executable(MapInfoLoaderTests tests/MapInfoLoaderTests.cpp src/MapInfoLoader.cpp)
    target_include_directories(MapInfoLoaderTests PRIVATE include)
    target_compile_definitions(MapInfoLoaderTests PRIVATE RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Resources")
    add_test(NAME map-loader COMMAND MapInfoLoaderTests "${CMAKE_BINARY_DIR}/test-fixtures/maps")

    add_executable(RuntimePathsTests tests/RuntimePathsTests.cpp src/RuntimePaths.cpp)
    target_include_directories(RuntimePathsTests PRIVATE include)
    add_test(NAME runtime-paths COMMAND RuntimePathsTests "${CMAKE_BINARY_DIR}/test-fixtures/runtime")
    add_executable(WorldTests tests/WorldTests.cpp)
    target_include_directories(WorldTests PRIVATE include)
    add_test(NAME world-lookup COMMAND WorldTests)
    add_executable(SceneLifetimeTests tests/SceneLifetimeTests.cpp)
    target_include_directories(SceneLifetimeTests PRIVATE include "${ptsd_SOURCE_DIR}/include")
    target_include_directories(SceneLifetimeTests SYSTEM PRIVATE ${DEPENDENCY_INCLUDE_DIRS})
    target_compile_definitions(SceneLifetimeTests PRIVATE RESOURCE_DIR="Resources")
    target_link_libraries(SceneLifetimeTests PRIVATE PTSD)
    add_test(NAME scene-lifetime COMMAND SceneLifetimeTests)
    add_test(NAME deployed-resources COMMAND ${CMAKE_COMMAND}
        "-DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}" "-DPTSD_SOURCE=${ptsd_SOURCE_DIR}"
        "-DDEPLOYED=$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/VerifyDeployment.cmake")
endif()
