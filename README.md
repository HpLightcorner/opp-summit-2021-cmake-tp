# Technical Preview for the OMNeT++ Community Summit 2021
This repository shows some advanced use cases for [the OMNeT++ CMake package](https://github.com/omnetpp/cmake), including:
- Managing of complex dependencies like INET
- Using functions like add_opp_target to automatically add an OMNeT++ structured folder as a CMake target
- Using add_opp_run to create a debug configuration for Visual Studio Code
- Managing CMake based dependencies 

## VS-Code Plugins
Developing with VS-Code is done easily with the following plugins:
- [_ms-vscode.cpptools_](https://github.com/microsoft/vscode-cpptools/) for C/C++ language support
- [_twxs.cmake_](https://github.com/twxs/vs.language.cmake) for CMake language support 
- [_ms-vscode.cmake-tools_](https://github.com/microsoft/vscode-cmake-tools) for CMake project integration 
- [_schrej.omnetpp-ned_](https://github.com/schrej/vscode-omnetpp-ned) for OMNeT++ NED language support 
- [_vadimcn.vscode-lldb_](https://github.com/vadimcn/vscode-lldb) (CodeLLDB) for debugging C++ applications

## Setting up the build environment on Windows with VS-Code
You simply can instruct the VS-Code CMake extension about your OMNeT++ compiler paths by adding a 'cmake-kits.json' file similar to the following example:

```json
[
    {
        "name": "CLang Omnet++ 6.10 with .venv",
        "environmentSetupScript": "${workspaceFolder}/.vscode/omnetpp-6.0pre10env.cmd", 
        "compilers": {
            "C": "path/to/omnetpp-6.0pre10/tools/win64/mingw64/bin/clang.exe",
            "CXX": "path/to/omnetpp-6.0pre10/tools/win64/mingw64/bin/clang++.exe"
        }
    }
]
```

where the script `omnetpp-6.0pre10env.cmd` is optional and can help to initialize the build environment:

```
set PATH=%PATH%;path\to\omnetpp-6.0pre10\tools\win64\mingw64\bin
set PATH=%PATH%;path\to\omnetpp-6.0pre10\bin
set PATH=%PATH%;path\to\omnetpp-6.0pre10\tools\win64\opt\mingw64\bin
set PATH=%PATH%;path\to\omnetpp-6.0pre10\tools\win64\usr\bin
set PATH=%PATH%;path\to\omnetpp-6.0pre10\lib

rem Optional: Activate a python virtual environment (CMake will then use the corresponding interpreter)
set current_dir="%~dp0"
call %current_dir%..\.venv\Scripts\activate.bat
```

