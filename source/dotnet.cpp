#include <string>
#include <comdef.h>
// #pragma comment(lib, "mscoree.lib")
// #import "mscorlib.tlb" auto_rename

#pragma region Includes and Imports
#include <metahost.h>
#include <cassert>
#pragma comment(lib, "mscoree.lib")

// Import mscorlib.tlb (Microsoft Common Language Runtime Class Library).
#import <mscorlib.tlb> raw_interfaces_only			\
    	high_property_prefixes("_get","_put","_putref")		\
    	rename("ReportEvent", "InteropServices_ReportEvent")	\
	rename("or", "InteropServices_or")
using namespace mscorlib;
#pragma endregion

_AssemblyPtr spAssembly = NULL;

extern "C" int InitializeDotnet(const void *rt, const void *assemblyDLL, const unsigned int assemblyDLL_len)
{
    //HRESULT hr;
    wchar_t* runtime=new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, static_cast<const char*>(rt), -1, runtime, 4096);

    HRESULT hr;
	ICLRRuntimeInfo* pRuntimeInfo = NULL;
	ICorRuntimeHost* pCorRuntimeHost = NULL;

	//----------------------------------------------------------------------
	// Load the CLR runtime
	ICLRMetaHost* pMetaHost = NULL;
	hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&pMetaHost));
	if (FAILED(hr)) {	
		return 0;
	}

	// Get the ICLRRuntimeInfo corresponding to a particular CLR version. It  
	// supersedes CorBindToRuntimeEx with STARTUP_LOADER_SAFEMODE. 
	hr = pMetaHost->GetRuntime(runtime, IID_PPV_ARGS(&pRuntimeInfo));
	if (FAILED(hr)) {
		return 0;
	}
	   
	// Check if the specified runtime can be loaded into the process. This  
	// method will take into account other runtimes that may already be  
	// loaded into the process and set pbLoadable to TRUE if this runtime can  
	// be loaded in an in-process side-by-side fashion.  
	BOOL fLoadable;
	hr = pRuntimeInfo->IsLoadable(&fLoadable);
	if (FAILED(hr))	{
		return 0;
	}
	if (!fLoadable)	{
		return 0;
	}

	// Load the CLR into the current process and return a runtime interface  
	// pointer. ICorRuntimeHost and ICLRRuntimeHost are the two CLR hosting   
	// interfaces supported by CLR 4.0. Here we demo the ICorRuntimeHost  
	// interface that was provided in .NET v1.x, and is compatible with all  
	// .NET Frameworks.  
	hr = pRuntimeInfo->GetInterface(CLSID_CorRuntimeHost, IID_PPV_ARGS(&pCorRuntimeHost));
	if (FAILED(hr))	{
		return 0;
	}

	//----------------------------------------------------------------------
	// Start the CLR
	hr = pCorRuntimeHost->Start();
	if (FAILED(hr))	{
		return 0;
	}

    //----------------------------------------------------------------------
	// Get the default AppDomain for this Runtime host
	IUnknownPtr spAppDomainThunk = NULL;
	_AppDomainPtr spDefaultAppDomain = NULL;
	
	// Get a pointer to the default AppDomain in the CLR.
	hr = pCorRuntimeHost->GetDefaultDomain(&spAppDomainThunk);
	if (FAILED(hr))	{
		return 0;
	}

	hr = spAppDomainThunk->QueryInterface(IID_PPV_ARGS(&spDefaultAppDomain));
	if (FAILED(hr))	{
		return 0;
	}

    SAFEARRAYBOUND bounds[1];
	bounds[0].cElements = assemblyDLL_len;
	bounds[0].lLbound = 0;

    SAFEARRAY* arr = SafeArrayCreate(VT_UI1, 1, bounds);
	SafeArrayLock(arr);
	memcpy(arr->pvData, assemblyDLL, assemblyDLL_len);
	SafeArrayUnlock(arr);

	hr = spDefaultAppDomain->Load_3(arr, &spAssembly);
	if (FAILED(hr))	{
		return 0;
	}

	return 1;
}






extern "C" int ManageDomain(const char *cls, const char *meth)
{
    HRESULT hr;
    wchar_t* dotnetMethod=new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, static_cast<const char*>(meth), -1, dotnetMethod, 4096);
	bstr_t bstrClassName(cls);
	_TypePtr spType = NULL;

	hr = spAssembly->GetType_2(bstrClassName, &spType);
	if (FAILED(hr))	{
		return -1;
	}



//----------------------------------------------------------------------
	// Finally, invoke the method passing it some arguments as a single string
	bstr_t bstrStaticMethodName(dotnetMethod);
	SAFEARRAY* psaStaticMethodArgs = NULL;
	variant_t vtStringArg(L"");
	variant_t vtPSEntryPointReturnVal;
	variant_t vtEmpty;


	psaStaticMethodArgs = SafeArrayCreateVector(VT_VARIANT, 0, 0);
	LONG index = 0;

	// Invoke the method from the Type interface.
	hr = spType->InvokeMember_3(
		bstrStaticMethodName,
		static_cast<BindingFlags>(BindingFlags_InvokeMethod | BindingFlags_Static | BindingFlags_Public),
		NULL,
		vtEmpty,
		psaStaticMethodArgs,
		&vtPSEntryPointReturnVal);

	if (FAILED(hr))	{
        _com_error err(hr);
        LPCTSTR errMsg = err.ErrorMessage();
        printf("%s\n", errMsg);
		return 0;
	}

	SafeArrayDestroy(psaStaticMethodArgs);
  	psaStaticMethodArgs = NULL;

    return 1;
}


extern "C" INT *CreateAppDomain(const char *d, const char *c, const char *config)
{
    HRESULT hr;
    wchar_t* domain=new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, static_cast<const char*>(d), -1, domain, 4096);
	wchar_t* configFile=new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, static_cast<const char*>(c), -1, configFile, 4096);
	bstr_t bstrClassName("ClrLoader.ClrLoader");
	_TypePtr spType = NULL;

	hr = spAssembly->GetType_2(bstrClassName, &spType);
	if (FAILED(hr))	{
		return 0;
	}

//----------------------------------------------------------------------
	// Finally, invoke the method passing it some arguments as a single string
	bstr_t bstrStaticMethodName(L"CreateAppDomain");
	SAFEARRAY* psaStaticMethodArgs = NULL;
	variant_t vtStringArg(domain);
	variant_t vtStringArg2(configFile);
	variant_t vtPSEntryPointReturnVal;
	variant_t vtEmpty;


	psaStaticMethodArgs = SafeArrayCreateVector(VT_VARIANT, 0, 2);
	LONG index0 = 0;
	LONG index1 = 1;

	hr = SafeArrayPutElement(psaStaticMethodArgs, &index0, &vtStringArg);
	if (FAILED(hr))	{
		return NULL;
	}
	hr = SafeArrayPutElement(psaStaticMethodArgs, &index1, &vtStringArg2);
	if (FAILED(hr))	{
		return NULL;
	}

	// Invoke the method from the Type interface.
	hr = spType->InvokeMember_3(
		bstrStaticMethodName,
		static_cast<BindingFlags>(BindingFlags_InvokeMethod | BindingFlags_Static | BindingFlags_Public),
		NULL,
		vtEmpty,
		psaStaticMethodArgs,
		&vtPSEntryPointReturnVal);

	if (FAILED(hr))	{
        _com_error err(hr);
        LPCTSTR errMsg = err.ErrorMessage();
        printf("%s\n", errMsg);
		return NULL;
	}

	SafeArrayDestroy(psaStaticMethodArgs);
  	psaStaticMethodArgs = NULL;
    return vtPSEntryPointReturnVal.pintVal;
}


extern "C" void * GetFunction(int *domainPtr, unsigned char *assemblyPath, const char *tName, const char *func, size_t asmSize)
{
    HRESULT hr;
	HRESULT asmSafeArrMod;
	SAFEARRAYBOUND asmContent[1];
	asmContent[0].cElements = asmSize;
	asmContent[0].lLbound   = 0;
	SAFEARRAY* asmSafeArr  = SafeArrayCreate(VT_UI1, 1, asmContent);
	void* asmPointer = NULL;
	asmSafeArrMod = SafeArrayAccessData(asmSafeArr, &asmPointer);
	if(FAILED(asmSafeArrMod)) {
		return 0;
	}
	memcpy(asmPointer, assemblyPath, asmSize);
	asmSafeArrMod = SafeArrayUnaccessData(asmSafeArr);
	if(FAILED(asmSafeArrMod)) {
		return 0;
	}
	wchar_t* typeName=new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, static_cast<const char*>(tName), -1, typeName, 4096);
	wchar_t* function=new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, static_cast<const char*>(func), -1, function, 4096);
	bstr_t bstrClassName("ClrLoader.ClrLoader");
	_TypePtr spType = NULL;

	hr = spAssembly->GetType_2(bstrClassName, &spType);
	if (FAILED(hr))	{
		return 0;
	}
	
//----------------------------------------------------------------------
	// Finally, invoke the method passing it some arguments as a single string
	bstr_t bstrStaticMethodName(L"GetFunction");
	SAFEARRAY* psaStaticMethodArgs = NULL;
	variant_t domainArg((int)domainPtr);
	//variant_t vtAsmArg(assemblyPath);
	variant_t vtAsmArg;
	vtAsmArg.vt = VT_ARRAY|VT_UI1;
	vtAsmArg.parray = asmSafeArr;



	variant_t vtTypeArg(typeName);
	variant_t vtFuncArg(function);
	variant_t vtPSEntryPointReturnVal;
	variant_t vtEmpty;

	psaStaticMethodArgs = SafeArrayCreateVector(VT_VARIANT, 0, 4);
	LONG index0 = 0;
	LONG index1 = 1;
	LONG index2 = 2;
	LONG index3 = 3;

	hr = SafeArrayPutElement(psaStaticMethodArgs, &index0, &domainArg);
	if (FAILED(hr))	{
		return NULL;
	}
	hr = SafeArrayPutElement(psaStaticMethodArgs, &index1, &vtAsmArg);
	if (FAILED(hr))	{
		return NULL;
	}
	hr = SafeArrayPutElement(psaStaticMethodArgs, &index2, &vtTypeArg);
	if (FAILED(hr))	{
		return NULL;
	}
	hr = SafeArrayPutElement(psaStaticMethodArgs, &index3, &vtFuncArg);
	if (FAILED(hr))	{
		return NULL;
	}
	// Invoke the method from the Type interface.
	hr = spType->InvokeMember_3(
		bstrStaticMethodName,
		static_cast<BindingFlags>(BindingFlags_InvokeMethod | BindingFlags_Static | BindingFlags_Public),
		NULL,
		vtEmpty,
		psaStaticMethodArgs,
		&vtPSEntryPointReturnVal);

	if (FAILED(hr))	{
        _com_error err(hr);
        LPCTSTR errMsg = err.ErrorMessage();
        printf("%s\n", errMsg);
		return NULL;
	}

	SafeArrayDestroy(psaStaticMethodArgs);
  	psaStaticMethodArgs = NULL;

    return vtPSEntryPointReturnVal.pintVal;
}


extern "C" int CloseAppDomain(int *domainPtr)
{
    HRESULT hr;
	bstr_t bstrClassName("ClrLoader.ClrLoader");
	_TypePtr spType = NULL;

	hr = spAssembly->GetType_2(bstrClassName, &spType);
	if (FAILED(hr))	{
		return 0;
	}

//----------------------------------------------------------------------
	// Finally, invoke the method passing it some arguments as a single string
	bstr_t bstrStaticMethodName(L"CloseAppDomain");
	SAFEARRAY* psaStaticMethodArgs = NULL;
	variant_t vtStringArg((int)domainPtr);
	variant_t vtPSEntryPointReturnVal;
	variant_t vtEmpty;


	//psaStaticMethodArgs = SafeArrayCreateVector(VT_UINT_PTR, 0, 1);
	psaStaticMethodArgs = SafeArrayCreateVector(VT_VARIANT, 0, 1);
	LONG index0 = 0;

	void *domainVoidPtr = (void *)(LONG)domainPtr;
	hr = SafeArrayPutElement(psaStaticMethodArgs, &index0, &vtStringArg);
	if (FAILED(hr))	{
		return NULL;
	}
	// Invoke the method from the Type interface.
	hr = spType->InvokeMember_3(
		bstrStaticMethodName,
		static_cast<BindingFlags>(BindingFlags_InvokeMethod | BindingFlags_Static | BindingFlags_Public),
		NULL,
		vtEmpty,
		psaStaticMethodArgs,
		&vtPSEntryPointReturnVal);

	if (FAILED(hr))	{
        _com_error err(hr);
        LPCTSTR errMsg = err.ErrorMessage();
        printf("%s\n", errMsg);
		return NULL;
	}

	SafeArrayDestroy(psaStaticMethodArgs);
  	psaStaticMethodArgs = NULL;

    return 1;
}

