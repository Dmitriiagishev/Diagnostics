cmake_minimum_required(VERSION 3.15)
project(MyCrowServer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Локальный Crow
add_library(Crow INTERFACE)
target_include_directories(Crow INTERFACE ${CMAKE_SOURCE_DIR}/vendor/crow/include)
target_compile_definitions(Crow INTERFACE CROW_USE_BOOST)
target_link_libraries(Crow INTERFACE pthread)

# Локальный Boost
set(BOOST_INCLUDEDIR ${CMAKE_SOURCE_DIR}/vendor/boost)
add_library(Boost_boost INTERFACE)
target_include_directories(Boost_boost INTERFACE ${BOOST_INCLUDEDIR})

add_executable(server 
    main.cpp 
    cpu.cpp    
    ram.cpp
    proc.cpp
)

target_link_libraries(server PRIVATE
    Crow
    Boost_boost
    pthread
)