# pyclrhost

pyclrhost is a custom wrapper for interfacing with a modified clr-loader dll from [pythonnet](https://github.com/pythonnet/clr-loader). This essentially means that, with my modified [clr-loader](https://github.com/rkbennett/clr-loader), you'll be able to import pythonnet from memory.

## Usage

After compiling this package via `python .\setup.py build` you will need to have also compiled my custom version of [clr-loader](https://github.com/rkbennett/clr-loader). Once you switch out their clrloader.dll for mine you'll be able to interact with it from memory

Basic PoC usage in python (The file paths will need updated as these are local to my system)

```python
import pyclrhost
dll = open(r"c:\Users\User\clr-loader\netfx_loader\bin\Release\net47\win10-x64\ClrLoader.dll","rb").read() #The raw bytes of my modified clrloader
pyclrhost.dotnet("v4.0.30319", dll) #Load the dll into the specified runtime
pyclrhost.pyclr_initialize() #Run the ClrLoader.ClrLoader's Initialize function
asm = open(r"c:\users\User\AppData\Local\Packages\PythonSoftwareFoundation.Python.3.10_qbz5n2kfra8p0\LocalCache\local-packages\Python310\site-packages\pythonnet\runtime\Python.Runtime.dll","rb").read() #the raw bytes of the pythonnet Python Runtime
intPtr = pyclrhost.pyclr_get_function(0, "Python.Runtime.Loader","Initialize", asm) #Get the intPtr of the Initialize function from Python.Runtime.Loader
intPtr #View the intPtr value
```

I will release a more complex usage example once I release an upcoming project I'm polishing up a bit more.

## Preamble

This is a continuation of my journey from my [memimporter](https://github.com/rkbennett/memimporter) research. I had figured out how to load the memimporter library from shellcode and started testing various python packages to see if they would work. I hit a snag when I came across pythonnet. The hurdle of import this has to do with the fact it's what's called a mixed-mode assembly. This essentially means it's a combination of managed (dotnet) and unmanaged (c, c++) code. This is a hard blocker for using MemoryModule, which is what _memimporter uses under the hood. I had to find another way! 

## Clr hosting

After trying numerous tools, doing a metric ton of googling, and getting a good intro to clr hosting and some examples from [bohops](https://github.com/bohops); I felt like clr hosting was going to be the most viable option. However, what I didn't anticipate was that it would require me to modify [pythonnet](https://github.com/pythonnet/clr-loader)'s clr-loader due to the fact that it is compiled as a mixed-mode assembly and, if I were to do clr hosting, I would need it to be a managed assembly.

## vTables..... why'd it have to be vTables....

At this point c# was somewhat new to me, so it took a little trial and error to get the clrloader to a place where it would compile for me as a managed assembly. The hardest part by far, though was writing the intermediary layer that this project performs, which leverages COM. Doing the translations to hand in various arguments as vTables was really something I don't think I knew what I was getting into when I started this project. However, I persisted and through *much* trial and error, was eventually able to start handing back int pointers, which was the first large mountain I had to climb. At this point I hit a last snag which had to do with the clrloader again. This issue was specifically related to the GetEntryPoint function of DataDomain, this was particularly difficult to me because of how the vTable had to be structured as well as how the vTable objects had to be cast by clrloader as it read them. Once this final obstacle was overcome, I finally had a working PoC.

## Gotchas

Keep in mind that I don't think the dotnet runtime is supposed to be interacted with in this way at all. I have seen one major issue that I think has more to do with how this works with pythonnet. The issue is that, when exiting python, if you have loaded a runtime, python will throw errors as it's unable to clean up the runtime appropriately. I've tried looking through pythonnet's code a bit more to see if there was a way I could fix it, but it may take someone more familiar with their code base than I. I just accept the error for now as it would be post execution anyway, so that runtime would already be dead at that point. It doesn't prevent you from starting a new python-hosted clr runtime.

## Final thoughts

I did add one extra goodie from what I learned during my work with [memimporter](https://github.com/rkbennett/memimporter), which is the ability to import it via shellcode, so if you read through that project, you should get a good idea of how to make that happen.

## Special Thanks

[bohops](https://github.com/bohops) for all the help with getting me off the ground with dotnet, as well as good starting points for clr hosting

[natesubra](https://github.com/natesubra) for being the rubber ducky during my troubleshooting
