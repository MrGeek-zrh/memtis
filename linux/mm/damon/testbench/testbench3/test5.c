#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define NUM_VMAS 100 // Number of VMAs to allocate
#define VMA_SIZE (1024) // Size of each VMA (1KB)
#define TARGET_VMA_INDEX 49 // Index of VMA to intensively access
#define INITIAL_PHASE_DUR 10 // Initial phase duration (seconds)
#define INTENSE_PHASE_DUR 5 // Intensive phase duration (seconds)
#define STEP_SIZE 64 // Access step size during intensive phase
#define ACCESS_INTERVAL 200000 // 200ms between accesses in initial phase

// Global variables
volatile sig_atomic_t phase = 0;
volatile char *vma_region[NUM_VMAS];

// Signal handler for phase transitions
void handler(int signum)
{
	phase++;
	alarm(phase == 1 ? INTENSE_PHASE_DUR : INITIAL_PHASE_DUR);
}

int main()
{
	struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	// Set up signal handler
	if (sigaction(SIGALRM, &sa, NULL) == -1) {
		perror("sigaction error");
		exit(EXIT_FAILURE);
	}

	// Allocate VMAs
	printf("Allocating %d VMAs, each %dKB\n", NUM_VMAS, VMA_SIZE / (1024));
	for (int i = 0; i < NUM_VMAS; i++) {
		vma_region[i] = mmap(NULL, VMA_SIZE, PROT_READ | PROT_WRITE,
				     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (vma_region[i] == MAP_FAILED) {
			perror("mmap failed");
			exit(EXIT_FAILURE);
		}
	}

	printf("Entering initial phase: sparse access (%d seconds)\n",
	       INITIAL_PHASE_DUR);
	alarm(INITIAL_PHASE_DUR); // Start first phase timer

	// Main access loop
	while (phase < 2) {
		if (phase == 0) {
			// Initial phase: sparse random access
			for (int i = 0; i < 5; i++) { // 5 accesses per second
				int vma_idx = rand() % NUM_VMAS;
				size_t offset = (rand() % (VMA_SIZE));
				vma_region[vma_idx][offset] = (char)time(NULL);
				usleep(ACCESS_INTERVAL);
			}
		} else if (phase == 1) {
			// Intensive phase: focused access on target VMA
			printf("\nEntering intensive phase: focused access on VMA[%d] "
			       "range:[%p-%p] (%d seconds)\n",
			       TARGET_VMA_INDEX, vma_region[TARGET_VMA_INDEX],
			       vma_region[TARGET_VMA_INDEX] + VMA_SIZE - 1,
			       INTENSE_PHASE_DUR);

			// Sequential access with STEP_SIZE stride
			char *target_vma = vma_region[TARGET_VMA_INDEX];
			for (size_t j = 0; j < VMA_SIZE; j++) {
				target_vma[j] = (char)(j);
				// Memory barrier to prevent optimization
				asm volatile("" ::: "memory");
			}
		}
	}

	// Cleanup phase
	printf("\nEntering cleanup phase: releasing all VMAs\n");
	for (int i = 0; i < NUM_VMAS; i++) {
		if (munmap(vma_region[i], VMA_SIZE) == -1) {
			perror("munmap failed");
		}
	}

	printf("Test completed\n");
	return 0;
}