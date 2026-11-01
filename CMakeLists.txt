cmake_minimum_required(VERSION 3.18)
project(ClueKeyboardFix LANGUAGES CXX)

if(NOT WIN32)
  message(FATAL_ERROR "This project can only be built on Windows.")
endif()

# The game is 32-bit, so the DLL must be x86.
# Configure with:  cmake -S . -B build -A Win32
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(FATAL_ERROR
    "The DLL must be 32-bit (x86). Configure with: cmake -S . -B build -A Win32")
endif()

# Static runtime (/MT): the DLL does not depend on the VC++ Redistributable.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Microsoft Detours (fetched from GitHub; the repository does not ship a
# CMakeLists.txt, so we build the library sources ourselves).
# You can pin a version with -DDETOURS_GIT_TAG=<tag or commit>.
# ---------------------------------------------------------------------------
include(FetchContent)

set(DETOURS_GIT_TAG "main" CACHE STRING "Detours tag/commit to use")

FetchContent_Declare(
  detours
  GIT_REPOSITORY https://github.com/microsoft/Detours.git
  GIT_TAG        ${DETOURS_GIT_TAG}
  GIT_SHALLOW    TRUE
  SOURCE_SUBDIR  _no_cmake   # prevents add_subdirectory
)
FetchContent_MakeAvailable(detours)

set(DETOURS_SRC "${detours_SOURCE_DIR}/src")

add_library(detours STATIC
  ${DETOURS_SRC}/detours.cpp
  ${DETOURS_SRC}/modules.cpp
  ${DETOURS_SRC}/disasm.cpp
  ${DETOURS_SRC}/image.cpp
  ${DETOURS_SRC}/creatwth.cpp
  ${DETOURS_SRC}/disolx86.cpp
  ${DETOURS_SRC}/disolx64.cpp
  ${DETOURS_SRC}/disolia64.cpp
  ${DETOURS_SRC}/disolarm.cpp
  ${DETOURS_SRC}/disolarm64.cpp
)
target_include_directories(detours PUBLIC ${DETOURS_SRC})
target_compile_definitions(detours PRIVATE WIN32_LEAN_AND_MEAN)

# ---------------------------------------------------------------------------
# The hook DLL (ClueKeyboardFix.cpp must be in the same folder as this file)
# ---------------------------------------------------------------------------
add_library(ClueKeyboardFix SHARED dllmain.cpp)
target_link_libraries(ClueKeyboardFix PRIVATE detours user32)
target_compile_definitions(ClueKeyboardFix PRIVATE WIN32_LEAN_AND_MEAN)
set_target_properties(ClueKeyboardFix PROPERTIES PREFIX "")   # produces ClueKeyboardFix.dll

# ---------------------------------------------------------------------------
# withdll.exe (Detours sample): launches a program with the DLL injected
# ---------------------------------------------------------------------------
option(BUILD_WITHDLL "Also build withdll.exe" ON)
if(BUILD_WITHDLL AND EXISTS "${detours_SOURCE_DIR}/samples/withdll/withdll.cpp")
  add_executable(withdll "${detours_SOURCE_DIR}/samples/withdll/withdll.cpp")
  target_link_libraries(withdll PRIVATE detours)
  target_compile_definitions(withdll PRIVATE WIN32_LEAN_AND_MEAN)
endif()