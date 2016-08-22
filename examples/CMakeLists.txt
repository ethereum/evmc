add_library(example-capi STATIC EXCLUDE_FROM_ALL capi.c)
target_include_directories(example-capi PRIVATE ../include)

add_library(example-vm STATIC EXCLUDE_FROM_ALL examplevm.c)
target_include_directories(example-vm PRIVATE ../include)
