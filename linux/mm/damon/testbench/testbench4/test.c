#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define T1_DURATION 5 // T1阶段持续时间(秒)
#define T2_DURATION 5 // T2阶段持续时间(秒)
#define REGION_SIZE (1 << 20) // 每个区域1MB大小(1048576字节)
#define ACCESS_INTENSITY 500000 // 每次操作的访问密度

/* 全局热点区域声明 */
volatile char a[REGION_SIZE];
volatile char b[REGION_SIZE];
volatile char c[REGION_SIZE];
volatile char d[REGION_SIZE];
volatile char e[REGION_SIZE];
volatile char f[REGION_SIZE];

/* 访问指定区域的辅助函数 */
void access_region(volatile char *region)
{
	// 随机访问模式,增加缓存不命中率
	for (int i = 0; i < ACCESS_INTENSITY; i++) {
		region[rand() % REGION_SIZE] = (char)(i % 256);
	}
}

/* 时间控制函数 */
void timed_operation(void (*phase)(void), int duration)
{
	time_t start = time(NULL);
	while (time(NULL) - start < duration) {
		phase();
	}
}

/* T1阶段访问模式 */
void t1_phase(void)
{
	access_region(a);
	access_region(b); // 重点访问b
	access_region(c);
	access_region(d);
	access_region(e);
}

/* T2阶段访问模式 */
void t2_phase(void)
{
	access_region(a);
	access_region(c);
	access_region(d);
	access_region(e);
	access_region(f); // 替换b为f
}

int main()
{
	srand(time(NULL)); // 初始化随机种子

	printf("测试开始 - T1阶段 (热点: a,b,c,d,e) \n");
	timed_operation(t1_phase, T1_DURATION);

	printf("进入T2阶段 (热点变为: a,c,d,e,f) \n");
	timed_operation(t2_phase, T2_DURATION);

	printf("测试结束\n");
	return 0;
}