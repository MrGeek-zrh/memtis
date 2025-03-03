#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define REGION_SIZE (1024 * 1024) // 分配1MB内存
#define LOOP_COUNT 1000           // 访存循环次数
#define STEP 64                   // 每次访问间隔64字节

// 使用volatile关键字防止编译器优化掉访存操作
volatile char *mem_region = NULL;

int main() {
  // 1. 分配内存区域
  mem_region = mmap(NULL, REGION_SIZE, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (mem_region == MAP_FAILED) {
    perror("mmap error");
    exit(EXIT_FAILURE);
  }
  printf("内存区域起始地址: %p, 大小: %d Bytes\n", (void *)mem_region,
         REGION_SIZE);

  // 2. 初始化内存区域
  for (size_t i = 0; i < REGION_SIZE; i++) {
    mem_region[i] = 0;
  }

  // 3. 顺序访存：遍历内存区域，每STEP字节写入一次数据
  printf("开始顺序访存测试...\n");
  for (int loop = 0; loop < LOOP_COUNT; loop++) {
    for (size_t i = 0; i < REGION_SIZE; i += STEP) {
      mem_region[i] = (char)((loop + i) % 256);
      // 如有需要，可记录或打印访问日志
      // printf("顺序：loop=%d, addr=%p, value=%d\n", loop, (void
      // *)&mem_region[i], mem_region[i]);
    }
    // 延时1ms，确保DAPTRACE可以捕获到中间状态
    usleep(1000);
  }
  printf("顺序访存测试结束.\n");

  // 4. 随机访存：随机访问内存区域内按照STEP排列的地址
  printf("开始随机访存测试...\n");
  srand(time(NULL));
  for (int loop = 0; loop < LOOP_COUNT; loop++) {
    size_t random_index = (rand() % (REGION_SIZE / STEP)) * STEP;
    mem_region[random_index] = (char)rand();
    // 如有需要，可记录或打印访问日志
    // printf("随机：loop=%d, addr=%p, value=%d\n", loop, (void
    // *)&mem_region[random_index], mem_region[random_index]);
    usleep(1000);
  }
  printf("随机访存测试结束.\n");

  // 5. 清理：释放内存区域
  if (munmap((void *)mem_region, REGION_SIZE) == -1) {
    perror("munmap error");
    exit(EXIT_FAILURE);
  }

  return 0;
}