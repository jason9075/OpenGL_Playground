cmake_minimum_required(VERSION 3.20)
project(OpenGLPlayground)

set(CMAKE_CXX_STANDARD 17)
set(OpenGL_GL_PREFERENCE "GLVND")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_package(GLEW 2.0 REQUIRED)
endif()
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
# Linux 會需要 threads 與 dl for PhysX
find_package(Threads REQUIRED)

# Include directories for external libraries
include_directories(include ${OPENGL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} external/imgui)

# --- imgui ---
set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/backends/imgui_impl_sdl2.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PRIVATE external/imgui)

# --- sources ---
file(GLOB_RECURSE GFX_CORE_SOURCES     src/core/*.cpp)
file(GLOB_RECURSE GFX_RESOURCE_SOURCES src/resource/*.cpp)
file(GLOB_RECURSE GFX_GEOM_SOURCES     src/geom/*.cpp)
file(GLOB_RECURSE GFX_RENDER_SOURCES   src/render/*.cpp)

add_library(gfx_core ${GFX_CORE_SOURCES})
target_include_directories(gfx_core PUBLIC include)

add_library(gfx_resource ${GFX_RESOURCE_SOURCES})
target_link_libraries(gfx_resource PUBLIC gfx_core)
target_include_directories(gfx_resource PUBLIC include)

add_library(gfx_geom ${GFX_GEOM_SOURCES})
target_link_libraries(gfx_geom PUBLIC gfx_core gfx_resource)
target_include_directories(gfx_geom PUBLIC include)

# 你的 ShaderClass 如果沒有獨立 target，先做個薄包裝（可放 ShaderClass.cpp 或只頭檔）：
add_library(ShaderLib INTERFACE)
target_include_directories(ShaderLib INTERFACE include)

add_library(gfx_render ${GFX_RENDER_SOURCES})
target_link_libraries(gfx_render PUBLIC gfx_geom ShaderLib)
target_include_directories(gfx_render PUBLIC include)

# --- sources app ---
file(GLOB APP_SOURCES src/*.cpp src/tests/*.cpp)
add_executable(playground.app Main.cpp ${APP_SOURCES})
target_precompile_headers(playground.app PRIVATE include/OPPCH.h)

# Disable tests and installation for nlohmann_json
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

# --- PhysX (import already-built static libs) ---
set(PHYSX_ROOT_DIR "${CMAKE_SOURCE_DIR}/external/physx/physx")
# 你實際輸出的位置：
set(PHYSX_OUT_DIR "${PHYSX_ROOT_DIR}/bin/linux.x86_64/release")
if(NOT EXISTS "${PHYSX_OUT_DIR}")
  message(FATAL_ERROR "找不到 PhysX 輸出目錄：${PHYSX_OUT_DIR}，請先執行 external/build_physx.sh 並編譯 Release。")
endif()

# 標頭 & 靜態庫 define
target_include_directories(playground.app PRIVATE "${PHYSX_ROOT_DIR}/include")
target_compile_definitions(playground.app PRIVATE PX_PHYSX_STATIC_LIB)

# 撈需要的靜態庫（注意 _static_64.a）
file(GLOB PHYSX_LIBS_FOUNDATION "${PHYSX_OUT_DIR}/libPhysXFoundation_static_64.a")
file(GLOB PHYSX_LIBS_COMMON     "${PHYSX_OUT_DIR}/libPhysXCommon_static_64.a")
file(GLOB PHYSX_LIBS_PHYSX      "${PHYSX_OUT_DIR}/libPhysX_static_64.a")
# 常用 helpers（PxCreatePlane 等）
file(GLOB PHYSX_LIBS_EXT        "${PHYSX_OUT_DIR}/libPhysXExtensions_static_64.a")
file(GLOB PHYSX_LIBS_COOKING    "${PHYSX_OUT_DIR}/libPhysXCooking_static_64.a")
file(GLOB PHYSX_LIBS_CCT        "${PHYSX_OUT_DIR}/libPhysXCharacterKinematic_static_64.a")

# 若之後要做 Collapse/Convex/Cooking 再開這個
# file(GLOB PHYSX_LIBS_COOKING    "${PHYSX_OUT_DIR}/libPhysXCooking_static_64.a")
# 先不連 PVD（會需要動態庫 libPVDRuntime_64.so 的 rpath）
# file(GLOB PHYSX_LIBS_PVD        "${PHYSX_OUT_DIR}/libPhysXPvdSDK_static_64.a")

# 匯入 targets
add_library(PhysXFoundation::static STATIC IMPORTED)
set_target_properties(PhysXFoundation::static PROPERTIES IMPORTED_LOCATION "${PHYSX_LIBS_FOUNDATION}")

add_library(PhysXCommon::static STATIC IMPORTED)
set_target_properties(PhysXCommon::static PROPERTIES IMPORTED_LOCATION "${PHYSX_LIBS_COMMON}")

add_library(PhysX::static STATIC IMPORTED)
set_target_properties(PhysX::static PROPERTIES IMPORTED_LOCATION "${PHYSX_LIBS_PHYSX}")

add_library(PhysXExtensions::static STATIC IMPORTED)
set_target_properties(PhysXExtensions::static PROPERTIES IMPORTED_LOCATION "${PHYSX_LIBS_EXT}")

if (EXISTS "${PHYSX_LIBS_COOKING}")
  add_library(PhysXCooking::static STATIC IMPORTED)
  set_target_properties(PhysXCooking::static PROPERTIES IMPORTED_LOCATION "${PHYSX_LIBS_COOKING}")
endif()

if (EXISTS "${PHYSX_LIBS_CCT}")
  add_library(PhysXCCT::static STATIC IMPORTED)
  set_target_properties(PhysXCCT::static PROPERTIES IMPORTED_LOCATION "${PHYSX_LIBS_CCT}")
endif()

# 若要用 Cooking:
# add_library(PhysXCooking::static STATIC IMPORTED)
# set_target_properties(PhysXCooking::static PROPERTIES IMPORTED_LOCATION "${PHYSX_LIBS_COOKING}")

# 若要用 PVD:
# add_library(PhysXPvdSDK::static STATIC IMPORTED)
# set_target_properties(PhysXPvdSDK::static PROPERTIES IMPORTED_LOCATION "${PHYSX_LIBS_PVD}")

target_link_options(playground.app PRIVATE "-Wl,--start-group")

# Link libraries
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(
        playground.app 
        gfx_render
        imgui 
        ${OPENGL_LIBRARIES} 
        ${SDL2_LIBRARIES} 
        GLEW::GLEW 
        nlohmann_json::nlohmann_json
        PhysXFoundation::static
        PhysXCommon::static
        PhysX::static
        PhysXExtensions::static
        # PhysXCooking::static
        # PhysXPvdSDK::static
    )
else()
    target_link_libraries(
        playground.app 
        gfx_render
        imgui 
        ${OPENGL_LIBRARIES} 
        ${SDL2_LIBRARIES} 
        GLEW 
        nlohmann_json::nlohmann_json
        PhysXFoundation::static
        PhysXCommon::static
        PhysX::static
        PhysXExtensions::static
        # PhysXCooking::static
        # PhysXPvdSDK::static
        Threads::Threads
        dl
    )
endif()


