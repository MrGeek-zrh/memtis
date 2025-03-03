#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define NUM_VMAS 100 // 定义 VMA 数量
#define VMA_SIZE (1024 * 1024) // 每个 VMA 分配 1MB 内存
#define ITERATIONS_PER_PHASE 3000 // 每个阶段内的循环次数
#define STEP 64 // 顺序访存时的步长

// 定义访存模式
typedef enum {
    ACCESS_SEQUENTIAL, // 顺序访存
    ACCESS_RANDOM, // 随机访存
    ACCESS_MIXED // 混合模式
} access_mode_t;

int main(void)
{
    // 使用 volatile 修饰防止编译器优化掉访存操作
    volatile char *vma[NUM_VMAS];

    // 分配多个 VMA 并初始化
    for (int i = 0; i < NUM_VMAS; i++) {
        vma[i] = mmap(NULL, VMA_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (vma[i] == MAP_FAILED) {
            perror("mmap error");
            exit(EXIT_FAILURE);
        }
        // 遍历初始化每个 VMA
        for (size_t j = 0; j < VMA_SIZE; j++) {
            vma[i][j] = 0;
        }
        printf("分配 VMA %d: 起始地址 %p, 大小 %d Bytes\n", i, (void *)vma[i], VMA_SIZE);
    }

    srand(time(NULL));

    printf("vma[1][0]:%lx,vma[0][0]%lx\n", &vma[1][0], &vma[0][0]);
    printf("&vma[1][0]-&vma[0][0]=%lx\n", &vma[1][0] - &vma[0][0]);

    // 定义三个不同阶段，对应不同的访存模式
    const int total_phases = 3;
    for (int phase = 0; phase < total_phases; phase++) {
        access_mode_t mode;
        if (phase == 0) {
            mode = ACCESS_SEQUENTIAL;
            printf("\n阶段 1：顺序访存\n");
        } else if (phase == 1) {
            mode = ACCESS_RANDOM;
            printf("\n阶段 2：随机访存\n");
        } else {
            mode = ACCESS_MIXED;
            printf("\n阶段 3：混合访存\n");
        }

        // 每个阶段内进行一定次数的访存操作
        for (int iter = 0; iter < ITERATIONS_PER_PHASE; iter++) {
            switch (mode) {
                case ACCESS_SEQUENTIAL:
                    // 遍历每个 VMA，按固定步长顺序访问
                    // vma[0...n][N],N = 0~16KB-1
                    for (int i = 0; i < NUM_VMAS; i++) {
                        size_t offset = (iter * STEP) % VMA_SIZE;
                        vma[i][offset] = (char)((i + iter) % 256);
                        // 可以选择打印日志，协助后续比对（测试时可注释掉减少干扰）
                        printf("顺序: vma[%d][%lu], value=%c\n", i, offset, vma[i][offset]);
                    }
                    break;
                case ACCESS_RANDOM: {
                    // 随机选择一个 VMA及随机偏移位置
                    int i = rand() % NUM_VMAS;
                    size_t offset = ((size_t)rand() * STEP) % VMA_SIZE;
                    vma[i][offset] = (char)(rand() % 256);
                    printf("随机: vma[%d][%lu], value=%c\n", i, offset, vma[i][offset]);
                    break;
                }
                case ACCESS_MIXED: {
                    // 随机决定是遍历所有 VMA 还是仅随机访问一个 VMA
                    if (rand() % 2 == 0) {
                        // 顺序方式：所有 VMA 顺序访问
                        for (int i = 0; i < NUM_VMAS; i++) {
                            size_t offset = (iter * STEP) % VMA_SIZE;
                            vma[i][offset] = (char)((i + iter) % 256);
                            printf("混合-顺序: vma[%d][%lu], value=%c\n", i, offset, vma[i][offset]);
                        }
                    } else {
                        // 随机方式：仅随机访问一个 VMA
                        int i = rand() % NUM_VMAS;
                        size_t offset = ((size_t)rand() * STEP) % VMA_SIZE;
                        vma[i][offset] = (char)(rand() % 256);
                        printf("混合-随机: vma[%d][%lu], value=%c\n", i, offset, vma[i][offset]);
                    }
                    break;
                }
            }
            // 延时，确保每次访存有足够的时间间隔，便于 DAPTRACE 采样
            usleep(1000);
        }
        printf("阶段 %d 结束.\n", phase + 1);
    }

    // 最后释放所有 VMA
    for (int i = 0; i < NUM_VMAS; i++) {
        if (munmap((void *)vma[i], VMA_SIZE) == -1) {
            perror("munmap error");
            exit(EXIT_FAILURE);
        }
    }

    printf("测试完成.\n");
    return 0;
}