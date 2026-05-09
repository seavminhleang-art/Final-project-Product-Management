cmake_minimum_required(VERSION 3.14)
project(ExcelTableApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# xlnt
FetchContent_Declare(
  xlnt
  GIT_REPOSITORY https://github.com/tfussell/xlnt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(xlnt)

# tabulate
FetchContent_Declare(
  tabulate
  GIT_REPOSITORY https://github.com/p-ranav/tabulate.git
  GIT_TAG master
)
FetchContent_MakeAvailable(tabulate)
file(GLOB_RECURSE XLNT_FILES
    "${xlnt_SOURCE_DIR}/source/*.cpp"
    "${xlnt_SOURCE_DIR}/include/*.hpp"
)


foreach(file ${XLNT_FILES})
    file(READ "${file}" CONTENT)
    string(FIND "${CONTENT}" "#include <cstdint>" HAS_CSTDINT)

    if(HAS_CSTDINT EQUAL -1)
        string(REPLACE "#pragma once" "#pragma once\n#include <cstdint>" CONTENT "${CONTENT}")
        file(WRITE "${file}" "${CONTENT}")
    endif()
endforeach()


add_executable(ExcelTableApp main.cpp)
add_executable(ExcelTableApp
    main.cpp
    src/Product.cpp
    src/ProductManager.cpp
    src/UserManager.cpp
    src/AuthService.cpp
    src/ReportTemplate.cpp
    src/Utils.cpp
)

target_include_directories(ExcelTableApp PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)


target_link_libraries(ExcelTableApp PRIVATE xlnt tabulate)