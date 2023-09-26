#include <Python.h>
#include <windows.h>

#include "MyDotnet.h"
#include "dotnet.h"

// #define VERBOSE /* enable to print debug output */

int MyInitializeDotnet(const void *runtime, unsigned char *bytes, const unsigned int size)
{
	BOOL *resp;
	resp = InitializeDotnet(runtime, bytes, size);
	return resp;
}

int MyManageDomain(const unsigned char *cls, const char *meth)
{
	BOOL *resp;
	resp = ManageDomain(cls, meth);
	return resp;
}

INT_PTR MyCreateAppDomain(const unsigned char *domain, const unsigned char *configFile, const char *meth)
{
	INT_PTR *resp;
	resp = CreateAppDomain(domain, configFile, meth);
	return resp;
}

void * MyGetFunction(int *domainPtr, unsigned char *assemblyPath, const unsigned char *typeName, const unsigned char *function, size_t asmSize)
{
	void *resp;
	resp = GetFunction(domainPtr, assemblyPath, typeName, function, asmSize);
	return resp;
}

int MyCloseAppDomain(int *domainPtr)
{
	BOOL *resp;
	resp = CloseAppDomain(domainPtr);
	return resp;
}
