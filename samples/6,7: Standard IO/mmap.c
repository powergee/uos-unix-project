#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>


int main(int argc, char *argv[])
{
	struct stat sb;
	off_t len;
	char *p;
	int fd;

	if( argc < 2 ) {
		printf(" %s <file>\n", argv[0]);
		return 1;
	}

	fd = open(argv[1], O_RDONLY);
	if( fd == -1 ) {
		perror("open");
		return 1;
	}

	if( fstat(fd, &sb) == -1 ) {
		perror("fstat"); 
		return 1;
	}

	p = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if( p == MAP_FAILED ) {
		perror("mmap");
		return 1;
	}

	close(fd);

	for(len = 0; len < sb.st_size; len++ )
		putchar(p[len]);

	munmap(p, sb.st_size);

	return 0;
}
