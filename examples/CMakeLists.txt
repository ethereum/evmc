add_library(example-vm STATIC examplevm.c)
target_include_directories(example-vm PRIVATE ../include)
target_compile_options(example-vm PRIVATE -Wno-extra)

add_executable(example-capi capi.c)
target_include_directories(example-capi PRIVATE ../include)
target_link_libraries(example-capi PRIVATE example-vm)
target_compile_options(example-capi PRIVATE -Wno-extra)
