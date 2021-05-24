#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void* mainFrameAddress;

#define SETUP mainFrameAddress = __builtin_frame_address(0); \
    printf("Main frame address: %lx\n", mainFrameAddress);

#define PRINT_STACK uint64_t a__ = 42;\
    void* frameEnd = __builtin_frame_address(0); \
    printf("%18s  %18s %18s %11s %11s\n", "address", "value (hex)", "value", "high", "low"); \
    printf("=================================================================================\n"); \
    for (uint64_t* ptr = &a__; ptr <= mainFrameAddress; ++ptr) { \
        printf("%18p  %18p %18lld %11d %11d\n", ptr, *ptr, *ptr, *((int*)ptr + 1), *((int*)ptr)); \
        if (ptr == frameEnd && ptr != mainFrameAddress) { \
            printf("================================== end of frame =================================\n"); \
            frameEnd = *((void**)frameEnd); \
        } \
    } \
    printf("=================================================================================\n");

void func() {
    long long b = 13;
    long long* ptr = &b;
    PRINT_STACK;
}

typedef struct MyStruct {
    int numbers[1024];
} MyStruct;

int main() {
    //MyStruct* s = malloc(sizeof(MyStruct));
    //MyStruct s;
    //int numbers[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int* numbers = malloc(8 * sizeof(int));
    printf("%lx\n", numbers);
    for (int i = 0; i < 8; ++i) {
        numbers[i] = i + 1;
    }
    SETUP;
    // PRINT_STACK;
    free(numbers);
    func();
}
