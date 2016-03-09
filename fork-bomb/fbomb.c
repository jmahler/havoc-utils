/*
 * fork bomb
 *
 * DESCRIPTION
 *
 * A fork bomb endlessly forks itself until something stops it.
 *
 *   (terminal 1)
 *   $ gcc -Wall -o fbomb fbomb.c
 *   $ ./fbomb
 *
 * To watch the progress the `watch` program along with `wc`
 * can be used to monitor the number of instances.
 *
 *   (terminal 2)
 *   $ watch -n 0.1 'ps aux | grep fbomb | wc -l'
 *
 * Another useful thing to watch is the amount of free memory.
 *
 *   (terminal 3)
 *   $ watch -n 0.5 free
 *
 * AUTHOR
 *
 * Jeremiah Mahler <jmmahler@gmail.com>
 * http://github.com/jmahler
 *
 * COPYRIGHT
 *
 * Copyright 2016, Jeremiah Mahler.  All Rights Reserved.
 * This project is free software and released under
 * the GNU General Public License.
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int pid;
	float rate;  /* forks / sec */

	char usage[] = "usage: fbomb <fork/sec>\n"
		       "       fbomb 1  # 1/sec\n";

	if (argc == 2) {
		sscanf(argv[1], "%f", &rate);

		if (rate <= 0) {
			fprintf(stderr, "rate must be > 0\n");
			fprintf(stderr, usage);
			return 1;
		}

		if (rate > 1e6) {
			fprintf(stderr, "rate must be < 1e6\n");
			fprintf(stderr, usage);
			return 1;
		}
	} else {
		fprintf(stderr, usage);
		return 1;
	}

	while (1) {
		pid = fork();
		if (pid) {
			waitpid(pid, NULL, 0);
			break;
		} else {
			usleep(1e6/rate);
			continue;
		}
	}

	return 0;
}
