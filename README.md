Graphical representation of Monte Carlo integration method

The project is cross-platform:
It was tested on Linux OS and Windows
It was tested on MacOS 10000 years ago. Few minor modification may be needed

Dependencies:
- SDL2 external library for graphics and keyboard/mouse inputs;
- cmake for build tool;

In order to run the project you need to have the listed libraries installed:
libsdl2-dev;
libsdl2-ttf-dev;
libsdl2-image-dev;
cmake;

1) To auto-generate na makefile step inside the build directory and run 'cmake ..' in the terminal;
1.1) You may need to specify a generator for your toolchain.
     For example to use MinGW on Windows you need to run 'cmake .. -G "MinGW Makefiles"' 
The cmake should generate a makefile.
2) After generation is complete run 'cmake --build .';
3) When compilation has completed run the binary with "./batman_integration";

Arguments of the binary:
- First: "number of points to evaluate". 
If no argument is provided - the default value of 2000000 points are used.

- Second: "--show-texts=yes" or "--show-texts=no"
The default value is "yes"
