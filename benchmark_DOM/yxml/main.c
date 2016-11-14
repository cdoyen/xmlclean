#define __USE_MINGW_ANSI_STDIO 1
#define _FILE_OFFSET_BITS  64  /* gcc,clang,mingw support for Files > 4GB on 32Bit also */

#include "yxml.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

unsigned char *readfile(const char *fn, unsigned long long *fsize)
{
	unsigned char *b;
	struct __stat64 s;
	FILE *f;
	if (_stat64(fn, &s)) perror(""), exit(1);
	b = (unsigned char*)calloc(1, s.st_size + 1);
	if (!b) perror(""), exit(2);
	f = fopen(fn, "rb");
	if (!f) perror(""), exit(3);
	if (!fread(b, s.st_size, 1, f))
		perror(""), exit(4);
	fclose(f);
	*fsize = s.st_size;
	return b;
}

void xmlbench(const char *buf, size_t bufsize) {
	yxml_t x[1];
	char stack[4096];
	yxml_init(x, stack, 4096);
	yxml_ret_t r;
	do {
		r = yxml_parse(x, *buf);
		buf++;
	} while (*buf && r >= 0);
	if (r >= 0)
		r = yxml_eof(x);
	/*printf("t%03lu l%03u b%03lu: %c %d", x->total, x->line, x->byte, *buf, r);*/
//	assert(!*buf && r == YXML_OK);
}

int main(int argc, char**argv)
{
	int i;
	clock_t sum = 0;
	unsigned long long fsize;
	for (i = 0; i < atoi(argv[2]); ++i)
	{
		unsigned char *xml = readfile(argv[1], &fsize);
		clock_t start = clock();

		{
			xmlbench((char*)xml, fsize);
		}

		sum += clock() - start;
		free(xml);
	}
	printf("%s\n%s\n%llu\n%.3f\n", argv[1], argv[2], fsize, sum / (double)CLOCKS_PER_SEC);
	return 0;
}
