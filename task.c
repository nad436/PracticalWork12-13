#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

void advanced_sigbus_handler(int signum, siginfo_t *info, void *context) {
    const char *reason = "Unknown SIGBUS reason";

    switch (info->si_code) {
        case BUS_ADRERR:
            reason = "Non-existent physical address or mmap file truncation (BUS_ADRERR)";
            break;
        case BUS_MCEERR_AR:
            reason = "Hardware memory error / Action Required (BUS_MCEERR_AR)";
            break;
        case BUS_MCEERR_AO:
            reason = "Hardware memory error / Action Optional (BUS_MCEERR_AO)";
            break;
        case BUS_ADRALN:
            reason = "Invalid address alignment (BUS_ADRALN)";
            break;
        case BUS_OBJERR:
            reason = "Object-specific hardware error (BUS_OBJERR)";
            break;
    }

    fprintf(stderr, "\n--- [SIGBUS Caught] ---\n");
    fprintf(stderr, "Signal Number: %d\n", signum);
    fprintf(stderr, "Faulting Address: %p\n", info->si_addr);
    fprintf(stderr, "Reason: %s\n", reason);
    fprintf(stderr, "-----------------------\n");

    exit(EXIT_FAILURE);
}

int main() {
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    act.sa_sigaction = advanced_sigbus_handler;
    act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGBUS, &act, NULL) < 0) {
        perror("sigaction failed");
        return 1;
    }

    printf("[*] SIGBUS handler registered successfully. PID: %d\n", getpid());
    printf("[*] Waiting for a signal (or trigger an error)...\n");

    while (1) {
        sleep(1);
    }

    return 0;
}
