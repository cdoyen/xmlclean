#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <irrXML.h>
using namespace irr;
using namespace io;

int main(int argc, char**argv)
{
	int i;
	clock_t sum = 0;

	for (i = 0; i < atoi(argv[2]); ++i)
	{
		IrrXMLReader* xml = createIrrXMLReader(argv[1]);

		clock_t start = clock();
		{
			while (xml->read())
			{
			}
		}
		sum += clock() - start;

		delete xml;
	}


	printf("%s\n%s\n%llu\n%.3f\n", argv[1], argv[2], 0, sum / (double)CLOCKS_PER_SEC);
	return 0;
}
