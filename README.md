# voxel_obj_lib_in_cpp
This is a simple libary for creating obj voxel files in cpp (tested only in ubuntu)
in the backend i use https://github.com/martinpflaum/multidimensional_array_cpp

view main.cpp to see how it's used. WorldGrid takes as first parameter the worldgridsize meaning x,y,z coordinates of 3Dworld.
The second parameter of WorldGrid is the size of the colorpallete x,y. so with a colorpallete of size 8 the textures you want to apply onto the object should be in a 8x8 grid. A 8x8 grid is for example 64x64 - each texture is 8x8 pixel. Its not that complicated just try some textures with same width and hight out.

in setitem the first 3 arguments are the x,y,z coordinate of the cube you want to change.
the 4th argument of setitem is the colorvalue of the voxel. -1 stands for void.
the obj() function returns the string that contains the objfile
 ```cpp
 auto worldGrid = WorldGrid(4,8);
 worldGrid.setitem(1,1,1,2);
 auto complete_file = worldGrid.obj();
 ```
