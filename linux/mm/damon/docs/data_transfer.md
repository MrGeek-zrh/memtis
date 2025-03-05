# DAMON Data Transfer Mechanism

DAMON (Data Access Monitoring) provides multiple ways to transfer sampled memory access data to user space and other kernel modules:

## User Space Interaction (debugfs interface)

### debugfs File Nodes
DAMON creates several debugfs file nodes in `dbgfs.c` for configuration and monitoring results:

- `kdamond_pid`: Shows the PID of monitoring thread
- `target_ids`: Displays list of monitored process IDs (PIDs) 
- `attrs`: Shows sampling interval (`sample_interval`), aggregation interval (`aggr_interval`) etc.

Users can write configurations via `echo` and read results via `cat` commands.

### Tracepoints
The `damon_aggregated` event (defined in `include/trace/events/damon.h`) is triggered when kernel aggregates data. Userspace tools like `perf` or `trace-cmd` can capture these events to get detailed information like region access counts.

Example output format: `target_id=123 nr_regions=5 0x1000-0x2000: 42 accesses`

## Kernel Module Collaboration

### Callback Mechanism
The `struct damon_callback` allows registering hooks like `after_aggregation`. Other modules can register callbacks to receive notifications and process data after aggregation.

For example, a memory compaction module could trigger page migration when cold memory regions are detected.

### Direct API Calls
DAMON provides `damon_target` and `damon_region` structures. Other modules can directly access monitoring data by traversing these structures.

For instance, the memory management subsystem could call `damon_for_each_region` to traverse regions and adjust page policies based on access frequency.

### mmu_notifier Integration
In vaddr.c, DAMON uses `mmu_notifier` to monitor virtual address space changes (like `mmap` or `munmap`), ensuring monitored regions stay consistent with actual mappings, indirectly collaborating with other memory management modules.

## Data Flow Example

1. **Sampling Phase**: DAMON thread (`kdamond`) periodically clears PTE access flags (via `damon_va_mkold`) to simulate "not accessed" state.
2. **Checking Phase**: On next sampling, it checks if PTE flags were set, counts accesses, and updates `nr_accesses` field.
3. **Aggregation & Output**: When aggregation interval is reached, data is reported via tracepoint for userspace tools to capture; debugfs files are also updated for users to read latest aggregated results.

## Configuration & Extensibility

- **Custom Callbacks**: Developers can extend `damon_callback` to implement custom data processing logic (e.g. forwarding data to Netlink sockets).
- **Module Parameters**: `damon_set_attrs` allows dynamic adjustment of monitoring parameters to adapt to different scenarios (e.g. real-time analysis vs long-term trend statistics).

In summary, DAMON provides flexible data access to userspace via debugfs and tracepoints, while enabling deep integration with memory management and other subsystems through callbacks and kernel APIs, forming an efficient data transfer mechanism.