cmake_minimum_required(VERSION 3.10)

project(DIC_Engine)

set(CMAKE_CXX_STANDARD 14)
if(MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED True)

set(OpenCV_DIR "D:/opencv/opencv/build/x64/vc16/lib")
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(DIC_App
    main.cpp 
    DICImage.cpp 
    DICSubset.cpp 
    FNCCSolver.cpp 
    ICGNSolver.cpp 
    DICPostProcessor.cpp
)

add_executable(Image_Gen
    ImageGenerator.cpp
)

target_link_libraries(DIC_App ${OpenCV_LIBS})
target_link_libraries(Image_Gen ${OpenCV_LIBS})
