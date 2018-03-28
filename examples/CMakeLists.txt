add_library(example-vm STATIC examplevm.c)
target_link_libraries(example-vm PRIVATE evmc)
target_compile_options(example-vm PRIVATE -Wno-extra)

add_executable(example-capi capi.c)
target_link_libraries(example-capi PRIVATE evmc example-vm)
target_compile_options(example-capi PRIVATE -Wno-extra)
