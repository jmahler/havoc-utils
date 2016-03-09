/*
 * memory leak
 *
 * DESCRIPTION
 *
 * Endlessly leak memory by calling malloc and discarding the pointer.
 *
 *   (terminal 1)
 *   $ gcc -Wall -o memleak memleak.c
 *   $ ./memleak 4096 1  # 4096 bytes at a rate of 1/sec
 *
 * To watch the progress, first the pid must be found.
 * Then the memory that that process is using can be viewed
 * using `pmap`.
 *
 *   (terminal 2)
 *   $ watch -n 0.4 'pmap 28927 | tail -n 1'
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

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	unsigned int bytes;
	float rate;

	char usage[] = "usage: memleak <bytes> <rate/sec>\n"
		       "       memleak 4096 1\n";

	if (argc == 3) {
		sscanf(argv[1], "%d", &bytes);

		if (bytes <= 0) {
			fprintf(stderr, "bytes must be > 0\n");
			fprintf(stderr, usage);
		}

		sscanf(argv[2], "%f", &rate);

		if (rate <= 0) {
			fprintf(stderr, "rate must be > 0\n");
			fprintf(stderr, usage);
		}
	} else {
		fprintf(stderr, usage);
		return 1;
	}

	while (1) {
		void *ptr;
			
		ptr = malloc(bytes);
		if (ptr == NULL) {
			perror("malloc");
			sleep(1);
		} else {
			if (rate <= 1e6)
				usleep((useconds_t) (1e6/rate));
		}
	}

	return 0;
}
