cmake_minimum_required(VERSION 3.10) 

project(HitsHaunter VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the ROOT package
find_package(ROOT REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# Include the header directories
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${ROOT_INCLUDE_DIRS}
                    ${EIGEN3_INCLUDE_DIR})

file(GLOB sources ${PROJECT_SOURCE_DIR}/source/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)

# Create the executable from source files
add_executable(haunter main.cpp ${sources} ${headers})

# Link the ROOT libraries to executable
target_link_libraries(haunter ${ROOT_LIBRARIES})
