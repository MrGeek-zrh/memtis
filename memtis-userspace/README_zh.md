
# MEMTIS 用户空间

## 实验复现指南

1) 将基准测试程序安装到 `./bench_dir` 目录

2) 创建 `./bench_cmds/[Benchmark].sh` 文件

3) 执行以下脚本获取基线性能：
   - `./run-all-nvm.sh`（预计耗时 7 小时 20 分钟）
   - `./run-all-nvm-fig7.sh`（预计耗时 15 小时）

4) 执行以下脚本生成各图表数据：
   - `./run-fig5-6-10.sh`（7 小时）
   - `./run-fig7.sh`（4 小时）
   - `./run-fig8-9.sh`（7 小时）
   - `./run-fig11.sh`（4 小时）
   - `./run-all-cxl.sh`（2 小时 15 分钟，需在服务器重启后以 CXL 模拟模式运行）

## 结果解析
使用 `parse-results.sh` 解析结果文件：
- 图5: `memtis-perf.dat`
- 图6: `results/${BENCH}/memtis-all/[内存配置]/hotness_stat.txt`
- 图7: `memtis-scalability.dat`
- 图8: `memtis-stat.dat`
- 图9: `results/[btree或silo]/memtis-all/1:8/throughput.out`
- 图10: `memtis-hitratio.dat`
- 图11: `memtis-cxl.dat`

## 其他系统配置技巧

### DRAM 容量限制
* **AutoNUMA/AutoTiering/Tiering-0.8/TPP**：使用 memmap 参数
  ```bash
  grubby --args="memmap=000M\!000M" --update-kernel=${内核路径或编号}
  ```
* **Nimble**：使用 memory cgroup 参数
* **HeMem**：需在编译时设置
  - 修改 `$DRAMSIZE` (hemem.h)
  - 必要时调整 `mmap_filter()` 中的长度阈值 (interpose.c)

### 各系统配置

#### HeMem
* 代码库: <https://bitbucket.org/ajaustin/hemem/src/sosp-submission/>
* 运行前需检查小内存分配导致的额外 DRAM 使用
* 示例配置（Btree 基准测试）:
  ```bash
  LD_PRELOAD=/path/libhemem.so numactl -N 0 [基准测试程序]
  ```

#### Nimble
* 内核代码: <https://github.com/ysarch-lab/nimble_page_management_asplos_2019>
* 用户空间代码: <https://github.com/ysarch-lab/nimble_page_management_userspace>
* 配置示例:
  ```bash
  # run_all.sh
  export FAST_NODE=0
  export SLOW_NODE=2
  BENCHMARK_LIST=需更新
  MEM_SIZE_LIST=需更新
  ```

#### AutoTiering
* 代码库: <https://github.com/csl-ajou/AutoTiering>
* 运行配置:
  ```bash
  ./run-bench.sh --benchmark [参数] -wss [参数] --max-threads 16 --iter 3 --socket 0
  ```

#### TPP
* 配置命令:
  ```bash
  sudo echo 1 > /sys/kernel/mm/numa/demotion_enabled
  sudo echo 3 > /proc/sys/kernel/numa_balancing
  ```

#### Tiering-0.8
* 内核代码: <https://git.kernel.org/pub/scm/linux/kernel/git/vishal/tiering.git/>
* 推荐配置:
  ```bash
  sudo echo 2 > /proc/sys/kernel/numa_balancing
  sudo echo 30 > /proc/sys/kernel/numa_balancing_rate_limit_mbps
  ```

#### AutoNUMA
* 启用命令:
  ```bash
  sudo echo 1 > /proc/sys/kernel/numa_balancing
  ```

> **注意事项**
> - 时间预估基于我们的测试环境，实际执行时间可能因系统配置有所不同
> - CXL 模拟模式需在服务器重启后运行