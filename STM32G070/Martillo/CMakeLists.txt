cmake_minimum_required (VERSION 3.5)
project (martillo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

#include_directories(../Inc) son flags globales

# m0_hal #
file(GLOB m0_hal_srcs "../Drivers/STM32G0xx_HAL_Driver/Src/*.c")
add_library(m0_hal ${m0_hal_srcs})
target_include_directories(m0_hal 
	"../Drivers/STM32G0xx_HAL_Driver/Inc")
	"../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy")
	"../Drivers/CMSIS/Include")
target_compile_flags(m0_hal
	
)

add_library(m0_bsp
	

)

add_executable (martillo
    main.cpp)


#Ejemplos
#if(Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#    target_link_libraries(martillo ${Boost_LIBRARIES})
#endif()

#target_include_directories (martillo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_link_libraries (stormbreaker ${OpenCV_LIBS} tensorflow_cc tensorflow_framework dlib::dlib caffe protobuf glog boost_system stdc++fs)
#target_link_libraries (martillo ${OpenCV_LIBS} boost_system boost_filesystem)


