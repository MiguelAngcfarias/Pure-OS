#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <errno.h>

int main() {
    int current_nice = getpriority(PRIO_PROCESS, 0);
    printf("Current process nice value: %d\n", current_nice);

    int result = nice(10); // Increase niceness (lower priority)
    if (result == -1 && errno != 0) {
        perror("nice failed");
        return 1;
    }

    int new_nice = getpriority(PRIO_PROCESS, 0);
    printf("New process nice value: %d\n", new_nice);

    // Dummy task to simulate process activity
    for (int i = 0; i < 5; i++) {
        printf("Running process work loop %d\n", i + 1);
        sleep(1);
    }

    return 0;
}
