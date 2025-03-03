#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#define ARRAY_SIZE (1024 * 1024) // 1MB 大小的数组
#define LOOP_COUNT 100000000 // 主循环次数

// 全局指针，指向映射的内存区域
volatile double *array_A; // 不间断访问的数组
volatile double *array_B; // 稍微间断访问的数组
volatile double *array_C; // 间断访问的数组

// 持续访问模式的函数：每次只访问固定的一个元素
void continuous_access_A(volatile double *arr, int size, int iteration)
{
    int index = 1;
    arr[index] += iteration * 1.0;
}

// 稍微间断访问模式的函数：每隔少数次数访问一个元素
void slightly_interrupted_access_B(volatile double *arr, int size, int iteration)
{
    // Start of Selection
    if (iteration % 2 == 0) {
        int index = 1;
        arr[index] += 1.0;
    }
}

// 间断访问模式的函数：每隔较多次数访问一个元素
void interrupted_access_C(volatile double *arr, int size, int iteration)
{
    if (iteration % 6 == 0) {
        int index = 1;
        arr[index] += 1.0;
    }
}

int main()
{
    // 通过 /dev/zero 使用 mmap 分配内存
    size_t map_size = ARRAY_SIZE * sizeof(double);

    array_A = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    array_B = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    array_C = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (array_A == MAP_FAILED || array_B == MAP_FAILED || array_C == MAP_FAILED) {
        perror("mmap failed");
        return 1;
    }

    // 初始化数组
    memset((void *)array_A, 0, map_size);
    memset((void *)array_B, 0, map_size);
    memset((void *)array_C, 0, map_size);

    printf("Array A address: %p\n", (void *)array_A);
    printf("Array B address: %p\n", (void *)array_B);
    printf("Array C address: %p\n", (void *)array_C);

    // 主循环
    for (int i = 0; i < LOOP_COUNT; i++) {
        // 不间断访问 A
        continuous_access_A(array_A, ARRAY_SIZE, i);
        /* // 稍微间断访问 B */
        /* slightly_interrupted_access_B(array_B, ARRAY_SIZE, i); */
        /**/
        /* // 间断访问 C */
        /* interrupted_access_C(array_C, ARRAY_SIZE, i); */
    }

    // 解除映射
    munmap((void *)array_A, map_size);
    munmap((void *)array_B, map_size);
    munmap((void *)array_C, map_size);

    return 0;
}
