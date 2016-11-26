#define __USE_MINGW_ANSI_STDIO 1
#define _FILE_OFFSET_BITS  64  /* gcc,clang,mingw support for Files > 4GB on 32Bit also */

#include "libxml/parser.h"
#include "libxml/tree.h"

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

void lstarttag(void *UserData, const unsigned char *name, const unsigned char **attrs)
{
}

void lendtag(void *UserData, const unsigned char *name)
{
}

void ltexttag(void *UserData, const unsigned char *data, int len)
{
}

static xmlSAXHandler my_handler =
{
	NULL, // internalSubset
	NULL, // isStandalone
	NULL, // hasInternalSubset
	NULL, // hasExternalSubset
	NULL, // resolveEntity
	NULL, // getEntity
	NULL, // entityDecl
	NULL, // notationDecl
	NULL, // attributeDecl
	NULL, // elementDecl
	NULL, // unparsedEntityDecl
	NULL, // setDocumentLocator
	NULL, // startDocument
	NULL, // endDocument
	lstarttag, // startElement
	lendtag, // endElement
	NULL, // reference
	ltexttag, // characters
	NULL, // ignorableWhitespace
	NULL, // processingInstruction
	NULL, // comment
	NULL, // warning
	NULL, // error
	NULL, // fatalError
	NULL, // getParameterEntity
	NULL, // cdataBlock
	NULL, // externalSubset
	0,
	NULL,
	NULL,
	NULL,
	NULL,
};

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
			if (xmlSAXUserParseMemory(&my_handler, NULL, xml, fsize) < 0)
				return 1;
			xmlCleanupParser();
		}

		sum += clock() - start;
		free(xml);
	}
	printf("%s\n%s\n%llu\n%.3f\n", argv[1], argv[2], fsize, sum / (double)CLOCKS_PER_SEC);
	return 0;
}

