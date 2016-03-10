/*
 * maxcon - max out connections to a server
 *
 * DESCRIPTION
 *
 * Endlessly establish connections with a server.
 *
 *   (terminal 1, client)
 *   $ ./maxcon localhost 51727
 *
 * To monitor its progress it is interesting to watch the
 * number of connections and the amount of free memory left.
 * These must be run on the server being connected to.
 *
 *   (terminal 2)
 *   $ watch -n 0.1 free
 *
 *   (terminal 3)
 *   $ watch -n 0.1 'netstat -an --ip | grep 51727 | grep ESTABLISHED | wc -l'
 *
 * Interestingly, the memory is consumed very slowly.  And the
 * number of new connections may be limited.  It may take a long
 * time to bring down a system in this way.
 *
 * You may also see messages like the following in the dmesg.
 *
 *   TCP: request_sock_TCP: Possible SYN flooding on port 51727.
 *     Dropping request.  Check SNMP counters.
 *
 * Strictly speaking, this program is not perform a SYN flood because
 * it is not breaking the TCP handshake, and it correctly establishes
 * a connection.  However, many SYNs are sent, so it can be seen why
 * this could be perceived as a SYN flood.
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

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

	struct addrinfo hints = {0};
	struct addrinfo *res = NULL;
	struct addrinfo *p;
	char *host;
	char *port;
	int n;
	int sockfd = 0;

	if (argc != 3) {
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		return 1;
	}
	host = argv[1];
	port = argv[2];

	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ( (n = getaddrinfo(host, port, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(n));
		return 1;
	}

	for (p = res; p != NULL; p = p->ai_next) {
		sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol);
		if (sockfd == -1) {
			perror("socket");
			continue;
		}

		break;
	}
	if (p == NULL) {
		fprintf(stderr, "No address could be used.\n");
		return 1;
	}

	/* endlessly establish connections */
	while (1) {
		n = connect(sockfd, p->ai_addr, p->ai_addrlen);
		if (n == -1) {
			perror("connect");
			sleep(1);
			continue;
		}

		/* new socket for next time around, leak the old one */
		while (1) {
			sockfd = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol);
			if (sockfd == -1) {
				perror("socket");
				sleep(1);
				continue;
			}
			break;
		}
	}

	return 0;
}
