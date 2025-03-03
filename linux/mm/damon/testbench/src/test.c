#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ARRAY_SIZE 1000000
#define ITERATIONS 3

void sequential_access(int *arr, int size) {
  printf("Sequential access...\n");
  for (int i = 0; i < size; i++) {
    arr[i] = i; // 连续写入
  }

  int sum = 0;
  for (int i = 0; i < size; i++) {
    sum += arr[i]; // 连续读取
  }
}

void random_access(int *arr, int size) {
  printf("Random access...\n");
  for (int i = 0; i < size; i += 100) {
    // 随机跳跃访问
    int idx = (i * 17 + 23) % size;
    arr[idx] = i;
    usleep(1000); // 延迟1ms，使访问模式更容易观察
  }
}

void pattern_access(int *arr, int size) {
  printf("Pattern access...\n");
  // 每隔8个元素访问一次
  for (int i = 0; i < size; i += 8) {
    arr[i] = i;
    usleep(100); // 延迟0.1ms
  }

  // 每隔16个元素访问一次
  for (int i = 0; i < size; i += 16) {
    arr[i] *= 2;
    usleep(100);
  }
}

int main() {
  printf("Allocating memory...\n");
  int *array = (int *)malloc(ARRAY_SIZE * sizeof(int));
  if (!array) {
    printf("Memory allocation failed!\n");
    return 1;
  }

  // 重复执行以产生足够的访存记录
  for (int i = 0; i < ITERATIONS; i++) {
    printf("\nIteration %d/%d\n", i + 1, ITERATIONS);
    sequential_access(array, ARRAY_SIZE);
    random_access(array, ARRAY_SIZE);
    pattern_access(array, ARRAY_SIZE);
  }

  free(array);
  printf("\nTest completed.\n");
  return 0;
}