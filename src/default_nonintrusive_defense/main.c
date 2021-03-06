#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "capdrop.h"
#include "child_control.h"
#include "rlimit_nproc.h"
#include "timeout.h"
#include "randenv.h"
#include "malloc.h"
#include "nobind.h"
#include "segv.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Usage: %s argv0 argv1\n", argv[0]);
        exit(1);
    }

    // This will fork, and kill all child processes after
    // the specified number of seconds.
    kill_timeout(120);

    // This prevents invoking the kill, setpgid, and setpgid syscalls,
    // which make the above "kill_timeout" effective against any process
    // we spawn.
    install_child_control();

    // Prevent the child from forking
    rlimit_nproc(0);

    // This drops all capabilities, so setuid binaries cannot be used.
    // This includes things like crontab and at.
    capdrop();

    // Make Use-After-Free harder to exploit
    set_malloc_flags();

    // Prevent the Global Offset Table (GOT) from being used
    // for a libc leak.
    set_got_nobind();

    // Generate a random-sized environment variable to break
    // stack offsets.
    randenv();

    // Log crashes to /tmp
    log_segv(argv[1]);

    execvp(argv[1], &argv[1]);
    perror("execvp");
}
