#ifndef GENERALLOADLIBRARY_H
#define GENERALLOADLIBRARY_H

BOOL MyInitializeDotnet(void *, unsigned char *, const unsigned int);

BOOL MyManageDomain(const unsigned char *, const char *);

INT_PTR MyCreateAppDomain(const unsigned char *, const unsigned char *, const char *);

void * MyGetFunction(int *, unsigned char *, const unsigned char *, const unsigned char *, size_t);

BOOL MyCloseAppDomain(int *);

#endif
