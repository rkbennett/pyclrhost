#include <windows.h>
#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC int InitializeDotnet(const void *rt, const void *bytes, const unsigned int size);

EXTERNC int ManageDomain(const char *cls, const char *meth);

EXTERNC int *CreateAppDomain(const char *domain, const char *configFile, const char *meth);

EXTERNC void *GetFunction(int *domainPtr, unsigned char *asmPath, const char *tName, const char *func, size_t);

EXTERNC int CloseAppDomain(int *domainPtr);

#undef EXTERNC