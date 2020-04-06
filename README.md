Graphical representation of Monte Carlo integration method.
List of developed heuristics: 
- Manhattan distance;
- Diagonal distance;
- Euclidean distance;

The project uses:

- Linux OS;
- SDL2 external library for graphics;
- cmake for build tool;

In order to run the project you need to have the listed libraries installed:
libsdl2-dev;
libsdl2-ttf-dev;
libsdl2-image-dev;
libsdl2-mixer-dev;
cmake;

1) To auto-generate na GNU makefile step inside the build directory and run "cmake .." in the terminal; 
The cmake should generate a GNU makefile.
2) After generation is complete run "make";
3) When compilation has completed run the binary with "./batman";

NOTE: the binary takes arguments:  
- First: "number of points to evaluate". If no argument is provided - the default value of 2000000 points are used.
- Second: "--show-texts=yes" or "--show-texts=no" - the default value is "yes"