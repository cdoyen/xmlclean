#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

using namespace std;

#include <iostream>
#include <string>

#ifndef ORAXML_CPP_ORACLE
# include <oraxml.hpp>
#endif

#pragma comment(lib, "oraxml10.lib")

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
			XMLParser   parser;
			ub4         flags;
			uword       ecode;
			flags = 0; // XML_FLAG_VALIDATE | XML_FLAG_DISCARD_WHITESPACE;

			if (ecode = parser.xmlinit())
			{
				cout << "Failed to initialize XML parser, error " << ecode;
				return 1;
			}

			if (ecode = parser.xmlparseBuffer(xml, fsize, (oratext *)0, flags))
				return 1;

			parser.xmlterm();	// terminate LPX package
		}

		sum += clock() - start;
		free(xml);
	}
	printf("%s\n%s\n%llu\n%.3f\n", argv[1], argv[2], fsize, sum / (double)CLOCKS_PER_SEC);
	return 0;
}

