#include "asm-xml.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/stat.h>

unsigned char *readfile(const char *fn, unsigned long long *fsize)
{
	unsigned char *b;
	struct __stat64 s;
	FILE *f;
	if (_stat64(fn, &s)) perror(""), exit(1);
	b = (unsigned char*)calloc(1, (std::size_t)s.st_size + 1);
	if (!b) perror(""), exit(2);
	f = fopen(fn, "rb");
	if (!f) perror(""), exit(3);
	if (!fread(b, (std::size_t)s.st_size, 1, f))
		perror(""), exit(4);
	fclose(f);
	*fsize = s.st_size;
	return b;
}

static const int  chunkSize = 16 * 1024 * 1024; // 16M
												//[c]
class BenchApplication
{
public:
	AXClassContext  classContext;
	AXElementClass* docClass;

	//[c]
	//[of]:  initializeAsmXml
	int initializeAsmXml(const char* schemaFilename)
	{
		// Initialize the AsmXml library
		ax_initialize((void*)malloc, (void*)free);

		// Initialize the class context
		int res = ax_initializeClassParser(&classContext);
		if (res != 0)
			return 1;

		// Read the schema and compile it
		unsigned long long ullsize;
		char* buf = (char*)readfile(schemaFilename, &ullsize);
		if (!buf)
			return 1;
		docClass = ax_classFromString(buf, &classContext);
		free(buf);
		if (docClass == NULL)
			return 1;

		return 0;
	}
	//[cf]
	//[of]:  releaseAsmXml
	void releaseAsmXml()
	{
		ax_releaseClassParser(&classContext);
	}
	//[cf]
	//[of]:  parseAsmXml
	//[c]Parses an XML string
	//[c]
	//[of]:  printAsmXmlError
	void printAsmXmlError(AXParseContext* context)
	{
		fprintf(stderr, "Error (%d,%d): %d\n",
			context->line,
			context->column,
			context->errorCode);
	}
	//[cf]
	//[c]
	int parseAsmXml(const char* buf)
	{
		AXParseContext parseContext;

		// Initialize the parser
		int res = ax_initializeParser(&parseContext, chunkSize);
		if (res != 0)
			return 1;

		AXElement* root = ax_parse(&parseContext, buf, docClass, 1);
		if (root == NULL)
		{
			printAsmXmlError(&parseContext);
			return 1;
		}

		ax_releaseParser(&parseContext);
		return 0;
	}
	//[cf]
	//[c]
};

int main(int argc, char**argv)
{
	int i;
	clock_t sum = 0;
	unsigned long long fsize;
	for (i = 0; i < atoi(argv[3]); ++i)
	{
		unsigned char *xml = readfile(argv[1], &fsize);
		clock_t start = clock();

		{
			BenchApplication bench;
			bench.initializeAsmXml(argv[2]);
			bench.parseAsmXml((char*)xml);
			bench.releaseAsmXml();
		}

		sum += clock() - start;
		free(xml);
	}
	printf("%s\n%s\n%llu\n%.3f\n", argv[1], argv[2], fsize, sum / (double)CLOCKS_PER_SEC);
	return 0;
}




