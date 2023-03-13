// malloc_ptr.cpp shows the use of std::unique_ptr to automatically
//                call free on memory allocated by malloc.
//
// build: cc malloc_ptr.cpp -o malloc_ptr
// use:   ./malloc_ptr
/////////////////////////////////////////////////////////////////////

#include <memory>   // std::unique_ptr
#include <stdio.h>  // printf perror
#include <stdlib.h> // realpath


// just a simple substitute of free() to give some feedback
static void verbose_free(void *ptr) {
    printf("  freeing %p\n", ptr);
    free(ptr);
}


// A simple functor to call free()
struct VerboseFree {
    void operator()(void *ptr) const { 
        printf("  freeing %p\n", ptr);
        free(ptr);
    }
};


// realpath uses malloc. this function calls free() automatically
void print_realpath(const char *file) {
    // all versions are working, just showing alternatives
    //std::unique_ptr<char, decltype(verbose_free)*> resolved_path(realpath(file, NULL), verbose_free);
    //std::unique_ptr<char, void(*)(void*)> resolved_path(realpath(file, NULL), verbose_free);
    std::unique_ptr<char, VerboseFree> resolved_path(realpath(file, NULL));
    if (resolved_path)
        printf("%s: %s\n", file, resolved_path.get());
    else
        perror(file);
}


int main(const int argc, const char **argv) {
    print_realpath(argv[0]);
    print_realpath(__FILE__);
    print_realpath("abcd.efg");

    return 0;
}
