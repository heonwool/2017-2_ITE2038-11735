#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFSIZE 1

int main() {
	int fd = open("data.db", O_RDONLY);
	int n;

	int64_t temp;


	while((n = read(fd, &temp, sizeof(temp))) > 0) {
		if (write(STDOUT_FILENO, &temp, n) != n) {
			printf("write error");
			exit(-1);
		}	
	}

	lseek(fd, 9, SEEK_SET);

	while((n = read(fd, &temp, sizeof(temp))) > 0) {
		if (write(STDOUT_FILENO, &temp, n) != n) {
			printf("write error");
			exit(-1);
		}	
	}
	

	return 0;
}
