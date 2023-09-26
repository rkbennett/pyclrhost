#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from importlib.machinery import EXTENSION_SUFFIXES
from setuptools import setup, find_packages, Extension

if sys.version_info < (3, 9):
    python_dll_name = '\\"python%d%d.dll\\"' % sys.version_info[:2]
    python_dll_name_debug = '\\"python%d%d_d.dll\\"' % sys.version_info[:2]
else:
    python_dll_name = '\"python%d%d.dll\"' % sys.version_info[:2]
    python_dll_name_debug = '\"python%d%d_d.dll\"' % sys.version_info[:2]


if "_d.pyd" in EXTENSION_SUFFIXES:
    macros = [("PYTHONDLL", python_dll_name_debug),
              ("_CRT_SECURE_NO_WARNINGS", '1')]
else:
    macros = [("PYTHONDLL", python_dll_name),
              ("_CRT_SECURE_NO_WARNINGS", '1'), ]

extra_compile_args = []
extra_link_args = []

extra_compile_args.append("-IC:\\Program Files\\Microsoft SDKs\\Windows\\v7.0\\Include")
extra_compile_args.append("-IC:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\include")
extra_compile_args.append("-IC:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.10586.0\\ucrt")
extra_compile_args.append("/DSTANDALONE")

if 0:
    # enable this to debug a release build
    extra_compile_args.append("/Od")
    extra_compile_args.append("/Z7")
    extra_link_args.append("/DEBUG")
    macros.append(("VERBOSE", "1"))

pyclrhost = Extension("pyclrhost",
                         ["source/pyclrhost.c",
                          "source/MyDotnet.c",
                          "source/dotnet.cpp"
                          ],
                         libraries=["user32", "shell32"],
                         define_macros=macros + [("STANDALONE", "1")],
                         extra_compile_args=extra_compile_args,
                         extra_link_args=extra_link_args,
                         )

ext_modules = []
if sys.platform.startswith("win32"):
    ext_modules = [pyclrhost]

with open("README.md", "r", encoding="utf-8") as f:
    long_desc = f.read()

setup(
    name='pyclrhost',
    version=0.1,
    packages=find_packages(),
    url='https://github.com/rkbennett/pyclrhost',
    license='MIT',
    author='R K Bennett',
    author_email='r.k.bennett@hotmail.com',
    description='Dotnet execution via clr hosting',
    long_description_content_type='text/markdown',
    python_requires='>=3.4',
    zip_safe=False,
    long_description=long_desc,
    ext_modules=ext_modules,
    classifiers=[
        'Development Status :: Alpha',

        'Environment :: Console',

        'License :: OSI Approved :: MIT License',

        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',

        'Programming Language :: Python :: Implementation :: CPython',
        'Programming Language :: Python :: Implementation :: PyPy',

        'Intended Audience :: Developers',
        'Intended Audience :: Information Technology',

        'Topic :: Software Development :: Libraries :: Python Modules',
        'Topic :: Software Development :: Build Tools',
        'Topic :: Software Development :: Testing',

    ],
    keywords=['clr',
              'dotnet',
              'clrhosting'],
)
