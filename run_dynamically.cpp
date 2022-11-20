#include <dlfcn.h>

int main() {
    void *handle = dlopen("/home/rosska/CLionProjects/labsLinuxOS/cmake-build-debug/libmy_dynamic_sentences_library.so", RTLD_LAZY | RTLD_GLOBAL);
    void (*runFunction)() = (void (*)()) dlsym(handle, "entry_function");
    (*runFunction)();
    dlclose(handle);
}