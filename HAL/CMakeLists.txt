add_library(HAL OBJECT)
target_include_directories(HAL PRIVATE Inc CMSIS/Include)
file(GLOB_RECURSE SOURCES "CMSIS/*.*" "Src/*.*" "Inc/*.*")
target_sources(HAL PRIVATE ${SOURCES})
