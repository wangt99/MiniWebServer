
# set header file path
set(PINE_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PINE_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)

# set compiler flag
set(CMAKE_COMPILER_FLAG -O3 -Wall -Wextra -pedantic -Werror)
#############################################################################
# Build lib
############################################################################

# Build core lib
file(GLOB PINE_CORE_SOURCES "${PROJECT_SOURCE_DIR}/src/core/*.cpp")
add_library(pine_core ${PINE_CORE_SOURCES})
target_link_libraries(pine_core PRIVATE pine_log)
target_include_directories(pine_core PUBLIC ${PINE_SRC_INCLUDE_DIR})
target_compile_options(pine_core PRIVATE ${CMAKE_COMPILER_FLAG})

# Build http lib
file (GLOB PINE_HTTP_SOURCES "${PROJECT_SOURCE_DIR}/src/http/*.cpp")
add_library(pine_http ${PINE_HTTP_SOURCES})
target_link_libraries(pine_http PRIVATE pine_log)
target_include_directories(pine_http PUBLIC ${PINE_SRC_INCLUDE_DIR})
target_compile_options(pine_http PRIVATE ${CMAKE_COMPILER_FLAG})

# Build logging lib
file(GLOB PINE_LOG_SOURCES "${PROJECT_SOURCE_DIR}/src/common/logger.cpp")
add_library(pine_log ${PINE_LOG_SOURCES})
target_include_directories(pine_log PUBLIC ${PINE_SRC_INCLUDE_DIR})
target_compile_options(pine_core PRIVATE ${CMAKE_COMPILER_FLAG})

# Build database lib
find_package(SQLiteCpp REQUIRED)
file(GLOB PINE_DB_SOURCES "${PROJECT_SOURCE_DIR}/src/db/*.cpp")
add_library(pine_db ${PINE_DB_SOURCES})
target_link_libraries(pine_db PRIVATE SQLiteCpp)
target_include_directories(pine_db PUBLIC ${PINE_SRC_INCLUDE_DIR})
target_compile_options(pine_db PRIVATE ${CMAKE_COMPILER_FLAG})
