#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/stat.h>  

// ConvertStringToBSTR
#include <comutil.h>  
#pragma comment(lib, "comsuppw.lib")

// MSXML6.0
#include <comdef.h>  
#include <msxml6.h>  
#include "msxml2.h"  
#pragma comment( lib, "msxml6.lib")  
_COM_SMARTPTR_TYPEDEF(IXMLDOMDocument2, __uuidof(IXMLDOMDocument2));
_COM_SMARTPTR_TYPEDEF(IXMLDOMParseError, __uuidof(IXMLDOMParseError));
// CLSIDs for MSXML objects (varies with MSXML version).  
#define DOMCLSID CLSID_DOMDocument60  

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

	HRESULT hr;

	CoInitialize(NULL);

	IXMLDOMDocument2 *pDOMDocument = NULL;
	hr = CoCreateInstance(DOMCLSID, NULL, CLSCTX_INPROC, IID_IXMLDOMDocument2, (LPVOID *)&pDOMDocument);
	if (FAILED(hr))
	{
		return perror("CoCreateInstance"), 1;
	}

	VARIANT_BOOL bSuccess;
	pDOMDocument->put_async(VARIANT_FALSE);
	pDOMDocument->put_resolveExternals(VARIANT_FALSE);
	pDOMDocument->put_validateOnParse(VARIANT_FALSE);

	for (i = 0; i < atoi(argv[2]); ++i)
	{
		unsigned char *xml = readfile(argv[1], &fsize);
		BSTR bstrText = _com_util::ConvertStringToBSTR((const char*)xml);
		bSuccess = 0;

		clock_t start = clock();
		{
			pDOMDocument->loadXML(bstrText, &bSuccess);
			if (VARIANT_FALSE == bSuccess)
				return perror("VARIANT_FALSE"), 1;
		}
		sum += clock() - start;

		free(xml);
	}

	pDOMDocument->Release();
	CoUninitialize();

	printf("%s\n%s\n%llu\n%.3f\n", argv[1], argv[2], fsize, sum / (double)CLOCKS_PER_SEC);
	return 0;
}
