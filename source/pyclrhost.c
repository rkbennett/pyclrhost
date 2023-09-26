// Need to define these to be able to use SetDllDirectory.
#define _WIN32_WINNT 0x0502
#define NTDDI_VERSION 0x05020000
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <windows.h>

static char module_doc[] =
"Importer which can do clrhosting";
char builtin_name[] = "builtins";

#include "MyDotnet.h"

/*
static int dprintf(char *fmt, ...)
{
	char Buffer[4096];
	va_list marker;
	int result;

	va_start(marker, fmt);
	result = vsprintf(Buffer, fmt, marker);
	OutputDebugString(Buffer);
	return result;
}
*/

#if (PY_VERSION_HEX < 0x03030000)
# error "Python 3.0, 3.1, and 3.2 are not supported"
#endif

#ifndef STANDALONE
extern wchar_t dirname[]; // executable/dll directory
#endif

//dotnet shiz
static PyObject *
dotnet(PyObject *self, PyObject *args)
{
	unsigned char *runtime;
	unsigned int size;
	unsigned char *data;
	BOOL resp;
	if (!PyArg_ParseTuple(args, "zs#",
			      &runtime, &data, &size))
		return NULL;
	resp = MyInitializeDotnet(runtime, data, size);
	Py_RETURN_NONE;
}

static PyObject *
pyclr_initialize(PyObject *self, PyObject *args)
{
	const unsigned char *cls = "ClrLoader.ClrLoader";
	const char *meth = "Initialize";
	BOOL resp;
	if (!PyArg_ParseTuple(args, ""
			      ))
		return NULL;
	resp = MyManageDomain(cls, meth);
	Py_RETURN_NONE;
}

static PyObject *
pyclr_finalize(PyObject *self, PyObject *args)
{
	const unsigned char *cls = "ClrLoader.ClrLoader";
	const char *meth = "Close";
	BOOL resp;
	if (!PyArg_ParseTuple(args, ""
			      ))
		return NULL;
	resp = MyManageDomain(cls, meth);
	Py_RETURN_NONE;
}

static PyObject *
pyclr_create_appdomain(PyObject *self, PyObject *args)
{
	const unsigned char *domain;
	const unsigned char *configFile;
	const char *meth = "Initialize";
	INT_PTR resp;
	if (!PyArg_ParseTuple(args, "s|s",
			      &domain, &configFile))
		return NULL;
	resp = MyCreateAppDomain(domain, configFile, meth);
	return PyLong_FromVoidPtr(resp);
}

static PyObject *
pyclr_close_appdomain(PyObject *self, PyObject *args)
{
	int *ptr;
	BOOL resp;
	void *domainPtr;
	if (!PyArg_ParseTuple(args, "i",
			      &ptr))
		return NULL;
	resp = MyCloseAppDomain(ptr);
	Py_RETURN_NONE;
}

static PyObject *
pyclr_get_function(PyObject *self, PyObject *args)
{
	int *ptr;
	void *resp;
	const unsigned char *assemblyName;
	const unsigned char *typeName;
	const unsigned char *function;

	unsigned char *raw;
	size_t size;

	if (!PyArg_ParseTuple(args, "isss#",
			      &ptr, &typeName, &function, &raw, &size))
		return NULL;
	resp = MyGetFunction(ptr, raw, typeName, function, size);
	return PyLong_FromVoidPtr(resp);
}

static PyMethodDef methods[] = {
	{ "dotnet", dotnet, METH_VARARGS,
	  "Initializes CLR in current process"},
	{ "pyclr_initialize", pyclr_initialize, METH_VARARGS,
	  "Initializes clr"},
	{ "pyclr_finalize", pyclr_finalize, METH_VARARGS,
	  "Closes clr"},
	{ "pyclr_create_appdomain", pyclr_create_appdomain, METH_VARARGS,
	  "Initializes domain in initiliazed clr"},
	{ "pyclr_close_appdomain", pyclr_close_appdomain, METH_VARARGS,
	  "Closes domain in initiliazed clr"},
	{ "pyclr_get_function", pyclr_get_function, METH_VARARGS,
	  "Gets function pointer to function delegate in initiliazed clr"},
	{ NULL, NULL },		/* Sentinel */
};

static struct PyModuleDef moduledef = {
	PyModuleDef_HEAD_INIT,
	"pyclrhost", /* m_name */
	module_doc, /* m_doc */
	-1, /* m_size */
	methods, /* m_methods */
	NULL, /* m_reload */
	NULL, /* m_traverse */
	NULL, /* m_clear */
	NULL, /* m_free */
};

PyMODINIT_FUNC PyInit_pyclrhost(void)
{
	return PyModule_Create(&moduledef);
};

INT APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		PyGILState_STATE gstate;
		gstate = PyGILState_Ensure();
        if (PyImport_AppendInittab("pyclrhost", PyInit_pyclrhost) == -1) {
            fprintf(stderr, "Error: could not extend in-built modules table\n");
            exit(1);
        }
		Py_Initialize();
		PyObject *Module = PyInit_pyclrhost();
		PyObject *builtin_module = PyImport_ImportModule(builtin_name);
		PyModule_AddObject(builtin_module, "pyclrhost", Module);
        PyGILState_Release(gstate);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return 1;
}