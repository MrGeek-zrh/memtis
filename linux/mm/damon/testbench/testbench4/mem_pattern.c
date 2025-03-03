#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define REGION_SIZE (1 << 20) // 每个区域1MB
#define T1_DURATION 10 // 第一阶段10秒
#define T2_DURATION 20 // 第二阶段20秒

// 内存区域定义
volatile char *regions[6]; // a(0),b(1),c(2),d(3),e(4),f(5)
volatile int dummy; // 防止编译器优化

void init_regions()
{
	for (int i = 0; i < 6; i++) {
		regions[i] = malloc(REGION_SIZE);
		for (int j = 0; j < REGION_SIZE; j++) {
			regions[i][j] = (char)(j % 256); // 初始化内存内容
		}
	}
}

void access_region(int idx)
{
	// 访问整个区域的不同位置
	for (int i = 0; i < REGION_SIZE; i += 4096) { // 按页访问
		dummy += regions[idx][i];
	}
}

int main()
{
	time_t start = time(NULL);
	init_regions();
	srand(time(NULL));

	printf("Test start (total %d seconds)\n", T1_DURATION + T2_DURATION);

	while (1) {
		time_t now = time(NULL) - start;
		if (now > T1_DURATION + T2_DURATION)
			break;

		// 第一阶段访问模式：重点访问a,b,c,d,e
		if (now <= T1_DURATION) {
			// 概率分布：a:20%, b:20%, c:20%, d:20%, e:20%, f:0%
			int r = rand() % 100;
			if (r < 20)
				access_region(0);
			else if (r < 40)
				access_region(1);
			else if (r < 60)
				access_region(2);
			else if (r < 80)
				access_region(3);
			else
				access_region(4);
		}
		// 第二阶段访问模式：重点访问a,c,d,e,f
		else {
			// 概率分布：a:20%, c:20%, d:20%, e:20%, f:20%, b:0%
			int r = rand() % 100;
			if (r < 20)
				access_region(0);
			else if (r < 40)
				access_region(2);
			else if (r < 60)
				access_region(3);
			else if (r < 80)
				access_region(4);
			else
				access_region(5);
		}

		usleep(1000); // 控制访问频率
	}

	// Cleanup
	for (int i = 0; i < 6; i++)
		free((void *)regions[i]);
	return 0;
}