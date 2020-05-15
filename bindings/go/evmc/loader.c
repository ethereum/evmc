/* Include evmc::loader in the Go package.
 * The "go build" builds all additional C/C++ files in the Go package directory,
 * but symbolic links are ignored so #include is used instead. */
#include "../../../lib/loader/loader.c"
