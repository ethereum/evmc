add_library(example-capi STATIC EXCLUDE_FROM_ALL capi.c)
target_include_directories(example-capi PRIVATE ../include)