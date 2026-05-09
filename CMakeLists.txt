cmake_minimum_required(VERSION 3.10)
project(aquarius)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)  # Отключаем GNU-расширения для переносимости

# Тип сборки по умолчанию, если не передан через -D
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Выберите тип сборки: Debug, Release, RelWithDebInfo, MinSizeRel" FORCE)
endif()


FILE (GLOB SOURCE_FILES
    ./*.c
    ./*.h
    )

add_executable(${PROJECT_NAME} ${SOURCE_FILES} )
