/***************************************************************************
 * Evil EarthCloud's Insidious Bomb, Version 1.0
 * Copyright 2026, Evil EarthCloud Incorporated. All rights reserved.
 *
 * LICENSE:
 *
 * Evil EarthCloud Incorporated (the PERPETRATOR) hereby grants you (the
 * VICTIM) explicit permission to use this bomb (the BOMB).  This is a
 * time limited license, which expires on the death of the VICTIM.
 * The PERPETRATOR takes no responsibility for damage, frustration,
 * insanity, bug-eyes, carpal-tunnel syndrome, loss of sleep, or other
 * harm to the VICTIM.  Unless the PERPETRATOR wants to take credit,
 * that is.  The VICTIM may not distribute this bomb source code to
 * any enemies of the PERPETRATOR.  No VICTIM may debug,
 * reverse-engineer, run "strings" on, decompile, decrypt, or use any
 * other technique to gain knowledge of and defuse the BOMB.  BOMB
 * proof clothing may not be worn when handling this program.  The
 * PERPETRATOR will not apologize for the PERPETRATOR's poor sense of
 * humor.  This license is null and void where the BOMB is prohibited
 * by law.
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "support.h"
#include "phases.h"

/*
 * Note to self: Remember to erase this file so my victims will have no
 * idea what is going on, and so they will all blow up in a
 * spectaculary fiendish explosion. -- Evil EarthCloud
 */

FILE *infile;

/* ------------------------------------------------------------------ */
/* Single-phase mode: run phase n alone, in a child process.           */
/*                                                                     */
/* The real phase code runs in a forked child, so a failure exits      */
/* through the usual explode_bomb() -> exit(8) path without taking     */
/* the parent (and the grader) down with it. The parent simply         */
/* reaps the child and reports PASS / FAILED.                          */
/*                                                                     */
/* This is what makes the five phases independent of one another:      */
/* a wrong phase 3 does not cost you phases 4 and 5.                   */
/* ------------------------------------------------------------------ */
static int run_one_phase(int n, char *input)
{
    pid_t pid;
    int status;

    fflush(stdout);
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(9);
    }

    if (pid == 0) {
        /* Child: stay quiet (explosions are swallowed here on purpose). */
        (void)freopen("/dev/null", "w", stdout);
        switch (n) {
        case 1: phase_1(input); break;
        case 2: phase_2(input); break;
        case 3: phase_3(input); break;
        case 4: phase_4(input); break;
        case 5: phase_5(input); break;
        default: exit(9);
        }
        exit(0);
    }

    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        exit(9);
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("[phase %d] PASS\n", n);
        fflush(stdout);
        return 0;
    }

    printf("[phase %d] FAILED\n", n);
    fflush(stdout);
    return 1;
}

int main(int argc, char *argv[])
{
    char *input;

    /* ------------------------------------------------------------------
     * Single-phase mode (used by the autograder; also handy for you when
     * you want to test one phase without replaying the whole chain):
     *
     *     ./bomb --phase 3            read one line from stdin, test phase 3
     *     ./bomb --phase 3 line3.txt  read one line from a file, test phase 3
     *
     * pass -> prints "[phase 3] PASS"   and exits 0
     * fail -> prints "[phase 3] FAILED" and exits 1
     * ------------------------------------------------------------------ */
    if (argc >= 3 && strcmp(argv[1], "--phase") == 0) {
        int n = atoi(argv[2]);

        if (n < 1 || n > NUM_PHASES) {
            printf("Usage: %s --phase <1..%d> [<input_file>]\n", argv[0], NUM_PHASES);
            exit(2);
        }

        infile = stdin;
        if (argc >= 4) {
            if (!(infile = fopen(argv[3], "r"))) {
                printf("%s: Error: Couldn't open %s\n", argv[0], argv[3]);
                exit(8);
            }
        }

        initialize_bomb();
        input = read_line();
        return run_one_phase(n, input);
    }

    /* ------------------------------------------------------------------
     * Classic mode: the five phases chained together, first explosion
     * wins -- this is the original "defusing a binary bomb" experience.
     * ------------------------------------------------------------------ */

    /* When run with no arguments, the bomb reads its input lines
     * from standard input. */
    if (argc == 1) {
        infile = stdin;
    }

    /* When run with one argument <file>, the bomb reads from <file>
     * until EOF, and then switches to standard input. Thus, as you
     * defuse each phase, you can add its defusing string to <file> and
     * avoid having to retype it. */
    else if (argc == 2) {
        if (!(infile = fopen(argv[1], "r"))) {
            printf("%s: Error: Couldn't open %s\n", argv[0], argv[1]);
            exit(8);
        }
    }

    /* You can't call the bomb with more than 1 command line argument. */
    else {
        printf("Usage: %s [<input_file>]\n", argv[0]);
        printf("       %s --phase <1..%d> [<input_file>]\n", argv[0], NUM_PHASES);
        exit(8);
    }

    /* Do all sorts of secret stuff that makes the bomb harder to defuse. */
    initialize_bomb();

    printf("Welcome to my fiendish little bomb. You have %d phases with\n", NUM_PHASES);
    printf("which to blow yourself up. Have a nice day!\n");

    input = read_line();  phase_1(input);  phase_defused();
    printf("Phase 1 defused. How about the next one?\n");

    /* The second phase is harder.  No one will ever figure out
     * how to defuse this... */
    input = read_line();  phase_2(input);  phase_defused();
    printf("That's number 2.  Keep going!\n");

    /* I guess this is too easy so far.  Some more complex code will
     * confuse people. */
    input = read_line();  phase_3(input);  phase_defused();
    printf("Three down, two to go!\n");

    /* Oh yeah?  Well, how good is your math?  Try on this saucy problem! */
    input = read_line();  phase_4(input);  phase_defused();
    printf("So you got that one.  Try this one.\n");

    /* Round and 'round in memory we go, where we stop, the bomb blows! */
    input = read_line();  phase_5(input);  phase_defused();

    return 0;
}
