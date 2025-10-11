#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

/**
 * Print current process information including PID and nice value
 */
void print_process_info(const char* stage) {
    pid_t pid = getpid();
    errno = 0; // Clear errno before getpriority call
    int nice_val = getpriority(PRIO_PROCESS, 0);
    
    if (errno != 0) {
        perror("getpriority failed");
        return;
    }
    
    printf("[%s] PID: %d, Nice value: %d (Priority: %s)\n", 
           stage, pid, nice_val, 
           nice_val < 0 ? "HIGH" : nice_val == 0 ? "NORMAL" : "LOW");
}

/**
 * Set process nice value with proper error handling
 */
int set_nice_value(int increment) {
    errno = 0; // Clear errno before nice call
    int result = nice(increment);
    
    // nice() returns -1 on both error and legitimate -1 nice value
    // Check errno to distinguish between them
    if (result == -1 && errno != 0) {
        fprintf(stderr, "Failed to set nice value (+%d): %s\n", 
                increment, strerror(errno));
        return -1;
    }
    
    return result;
}

/**
 * Simulate CPU-intensive work
 */
void simulate_work(int iterations, int sleep_seconds) {
    printf("\n--- Simulating %d work iterations (sleeping %ds each) ---\n", 
           iterations, sleep_seconds);
    
    for (int i = 0; i < iterations; i++) {
        printf("Work iteration %d/%d - Process running with current priority\n", 
               i + 1, iterations);
        
        // Simulate some CPU work
        volatile long sum = 0;
        for (long j = 0; j < 1000000; j++) {
            sum += j;
        }
        
        if (sleep_seconds > 0) {
            sleep(sleep_seconds);
        }
    }
}

/**
 * Display help information
 */
void print_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Options:\n");
    printf("  -n <value>    Set nice increment (default: 10)\n");
    printf("  -i <count>    Number of work iterations (default: 5)\n");
    printf("  -s <seconds>  Sleep duration between iterations (default: 1)\n");
    printf("  -h            Show this help message\n");
    printf("\nNice values range from -20 (highest priority) to 19 (lowest priority)\n");
    printf("Only root can decrease nice values (increase priority)\n");
}

int main(int argc, char* argv[]) {
    int nice_increment = 10;
    int work_iterations = 5;
    int sleep_duration = 1;
    int opt;
    
    // Parse command line arguments
    while ((opt = getopt(argc, argv, "n:i:s:h")) != -1) {
        switch (opt) {
            case 'n':
                nice_increment = atoi(optarg);
                if (nice_increment < -20 || nice_increment > 19) {
                    fprintf(stderr, "Warning: Nice increment %d is outside typical range [-20, 19]\n", 
                            nice_increment);
                }
                break;
            case 'i':
                work_iterations = atoi(optarg);
                if (work_iterations <= 0) {
                    fprintf(stderr, "Error: Work iterations must be positive\n");
                    return EXIT_FAILURE;
                }
                break;
            case 's':
                sleep_duration = atoi(optarg);
                if (sleep_duration < 0) {
                    fprintf(stderr, "Error: Sleep duration cannot be negative\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            case '?':
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }
    
    printf("=== Process Priority Management Demo ===\n");
    
    // Show initial process state
    print_process_info("INITIAL");
    
    // Attempt to change nice value
    printf("\nAttempting to adjust nice value by %+d...\n", nice_increment);
    if (set_nice_value(nice_increment) == -1) {
        fprintf(stderr, "Failed to change process priority. Continuing with current priority.\n");
    } else {
        printf("Successfully adjusted process priority!\n");
    }
    
    // Show new process state
    print_process_info("AFTER ADJUSTMENT");
    
    // Simulate process work
    simulate_work(work_iterations, sleep_duration);
    
    // Final status
    print_process_info("FINAL");
    
    printf("\n=== Demo completed successfully ===\n");
    return EXIT_SUCCESS;
}
