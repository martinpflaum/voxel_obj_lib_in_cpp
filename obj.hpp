#include <iostream>
#include <string.h>
#include <tuple>
#include <list>

//#include "Array.hpp"
//#include "ObjectExperimentalWorking.hpp"

#include "Array.hpp"
#include "ObjectExperimental.hpp"

using string = std::string;

Tuple splitt_face(Array<int>& face){
    Array<int> lsg_left = zeros_like(face);
    Array<int> lsg_up = zeros_like(face);
    Array<int> is_end = zeros_like(face) + 1;
    
    for(int row = 0; row < face.shape(0); row++){
        for(int col = 0; col < face.shape(1); col++){
            if(row == 0 && col == 0){
                is_end[row][col] = 1;
            }else if(row == 0){
                if(face[row][col-1] == face[row][col] && is_end[row][col - 1]){
                    is_end[row][col-1] = 0;
                    is_end[row][col] = 1;

                    lsg_left[row][col] = lsg_left[row][col-1] + 1;
                }else{
                    is_end[row][col] = 1;
                }
            }else if(col == 0){
                if(face[row-1][col] == face[row][col] && is_end[row-1][col]){
                    is_end[row-1][col] = 0;
                    is_end[row][col] = 1;

                    lsg_up[row][col] = lsg_up[row-1][col] + 1;
                }else{
                    is_end[row][col] = 1;
                }
            }else{
                if(face[row-1][col]==face[row][col] && is_end[row-1][col] && face[row][col-1] == face[row][col] && is_end[row][col - 1] && (lsg_left[row][col-1] + 1) == lsg_left[row -1][col]){
                    is_end[row-1][col] = 0;
                    is_end[row][col-1] = 0;
                    is_end[row][col] = 1;

                    lsg_left[row][col] = lsg_left[row-1][col];
                    lsg_up[row][col] = lsg_up[row-1][col] + 1;
                }else if(lsg_up[row][col-1] == 0 && face[row][col-1] == face[row][col] && is_end[row][col-1] == 1){
                    lsg_left[row][col] = lsg_left[row][col-1] + 1;
                    is_end[row][col-1] = 0;
                    is_end[row][col] = 1;
                }else if(lsg_left[row - 1][col] == 0 && face[row - 1][col] == face[row][col] && is_end[row-1][col] == 1){
                    lsg_up[row][col] = lsg_up[row-1][col] + 1;
                    is_end[row - 1][col] = 0;
                    is_end[row][col] = 1;
                }else{
                    is_end[row][col] = 1;
                }
            }
        }
    }
    return Tuple(lsg_left,lsg_up,is_end);
}

//std::tuple<Array<int>,Array<int>,Array<int>> splitt_face(Array<int>& face){

Tuple get_idx(Array<int>& lsg_left,Array<int>& lsg_up,Array<int>& is_end,Array<int>& face){
    int out_len = 0;
    for(int row = 0; row < face.shape(0); row++){
        for(int col = 0; col < face.shape(1); col++){
            if(is_end[row][col] == 1 && face[row][col] != 0){
                out_len++;
            }
        }
    }

    Array<int> points = Array<int>(out_len,4,2);
    Array<int> color = Array<int>(out_len);
    Array<bool> clockwise = Array<bool>(out_len);

    int i = 0;
    for(int row = 0; row < face.shape(0); row++){
        for(int col = 0; col < face.shape(1); col++){
            if(is_end[row][col] == 1 && face[row][col] != 0){
                points[i][0][0] = row - lsg_up[row][col];
                points[i][0][1] = col - lsg_left[row][col];

                points[i][1][0] = row - lsg_up[row][col];
                points[i][1][1] = col + 1;
                
                points[i][2][0] = row + 1;
                points[i][2][1] = col + 1;

                points[i][3][0] = row + 1;
                points[i][3][1] = col - lsg_left[row][col];
                
                if(face[row][col] < 0){
                    clockwise[i] = true;
                    color[i] = (-face[row][col]) -1;
                }else{
                    clockwise[i] = false;
                    color[i] = (face[row][col]) -1;
                }
            }
        }
    }
    return Tuple(points,color,clockwise);
}



class PolygonGrid{
public:
    int dim = 0;
    int dim_idx = 0;

    int size;
    Array<int> edge_lines;
    string out = "";
    int mul = 1;
    int current_line = 1;

    PolygonGrid(int _size) : size(_size) {
        edge_lines = Array<int>(size+1,size+1,size+1) - 1;
    }

    int get_edge_line(int row,int col){
        if(dim == 0){
            return edge_lines[dim_idx][row][col];
        }
        if(dim == 1){
            return edge_lines[row][dim_idx][col];
        }
        if(dim == 2){
            return edge_lines[row][col][dim_idx];
        }
        std::cout << "Error in polygonGrid\n";
        return -1;
    }

    void set_edge_line(int row,int col,int value){
        if(dim == 0){
            out += "v " + str(dim_idx * mul) + " " + str(row * mul)  + " " + str(col * mul) + "\n";
            edge_lines[dim_idx][row][col] = value;
        }
        if(dim == 1){
            out += "v " +  str(row * mul) + " " + str(dim_idx * mul)  + " " + str(col * mul) + "\n";
            edge_lines[row][dim_idx][col] = value;
        }
        if(dim == 2){
            out += "v " +  str(row * mul) +" " + str(col * mul)  +  " " + str(dim_idx * mul) + "\n";
            edge_lines[row][col][dim_idx] = value;
        }   
    }

    int get_line(int row,int col){
        if(get_edge_line(row,col) != -1){
            return this->get_edge_line(row,col);
        }else{
            this->set_edge_line(row,col,current_line);
            current_line = current_line + 1;
            return this->get_edge_line(row,col);
        }
    }
};

string create_color_line(Array<int>& vecs){
    string out = "";
    for(int i = 0; i < vecs.shape(0);i++){
        out += "vt " + str(int(vecs[i][0])) + " " + str(int(vecs[i][1])) + "\n";
    }
    return out;
}

class ColorGrid{
public:
    int size;
    Array<int> color_lines;
    int current_line = 0;
    string out;

    ColorGrid(int _size) : size(_size) {
        color_lines = Array<int>(_size,_size) - 1;
    }

    int get_line(int idx,int edge){
        if(color_lines[idx % size][idx / size] == -1){
            int x = idx / size;
            int y = idx % size;
            
            Array<int> tmp = {{x/size,y/size},
                            {x/size+1/size,y/size},
                            {x/size+1/size,y/size + 1/size},
                            {x/size,y/size+1/size}};
            out += create_color_line(tmp);
            color_lines[idx % size][idx / size] = current_line;
            current_line++;
            return color_lines[idx % size][idx / size] * 4 + edge + 1;
        }else{
            return color_lines[idx % size][idx / size] * 4 + edge + 1;
        }
    }
};

string create_faces(PolygonGrid& polygonGrid,ColorGrid& colorGrid,Array<int>& points,Array<int>& color_array,Array<bool>& clockwise_array){
    string out = "";
    int group = polygonGrid.dim;
    
    for(int j = 0; j < points.shape(0);j++){
        Array<int> fpoint = Array<int>(points[j]).reshape(4,2);
        int color = color_array[j];
        bool clockwise = clockwise_array[j];

        int finalGroup = 0;
        if(clockwise){
            finalGroup = 2*group + 1;
        }else{
            finalGroup = 2*group + 2;
        }


        Array<int> wline_array = Array<int>(4);
        Array<int> cline_array = Array<int>(4);

        for(int k = 0;k < 4;k++){
            //std::cout << fpoint[k][0] << " " <<fpoint[k][1] << "\n";
            int wline = polygonGrid.get_line(fpoint[k][0],fpoint[k][1]);
            int cline = colorGrid.get_line(color,k);
            wline_array[k] = wline;
            cline_array[k] = cline;
        }

        if(clockwise){
            wline_array._reverse_data();
            cline_array._reverse_data();
        }
        
        out += "s "+str(finalGroup) + "\nf ";
        
        for(int k = 0;k< 4;k++){
            out += str(int(wline_array[k])) + "/" + str(int(cline_array[k])) + "/" + str(finalGroup) + " ";
        }
        out += "\n";
    }
    return out;
}
/*
    template<class... Args>
    Test(int a,Args const &... args) : Object(type(Test)), x(a) {
        //auto tmp = static_cast<Test>(init);
        //std::cout << "tmp "<< tmp.x << "\n"; 
    }
*/

bool cube_valid(int size,int x,int y,int z){
    if(x >= size or y >= size or z >= size){
        return false;
    }
    if(x < 0 or y < 0 or z < 0){
        return false;
    }
    return true;
}

class WorldGrid{
public:
    Array<int> voxel;
    Array<int> xfaces;
    Array<int> yfaces;
    Array<int> zfaces;
    int size;
    int color_size;

    WorldGrid(int _size,int _color_size){
        size = _size;
        color_size = _color_size;
        voxel = Array<int>(size,size,size) - 1;
        xfaces = Array<int>(size + 1,size,size);
        yfaces = Array<int>(size,size + 1,size);
        zfaces = Array<int>(size,size,size + 1);
    }

    bool cube_not_empty(int x,int y,int z){
        if(!cube_valid(size,x,y,z)){
            return false;
        }else{
            return voxel[x][y][z] > 0;
        }
    }

    void setitem(int x, int y,int z,int item){
        voxel[x][y][z] = item;
        if(item == -1){
            if(cube_not_empty(x-1,y,z)){
                xfaces[x][y][z] = voxel[x-1][y][z] * (-1);
            }
            if(cube_not_empty(x+1,y,z)){
                xfaces[x+1][y][z] = voxel[x+1][y][z] * (1);
            }

            if(cube_not_empty(x,y-1,z)){
                yfaces[x][y][z] = voxel[x][y-1][z] * (1);
            }
            if(cube_not_empty(x,y+1,z)){
                yfaces[x][y+1][z] = voxel[x][y+1][z] * (-1);
            }


            if(cube_not_empty(x,y,z-1)){
                zfaces[x][y][z] = voxel[x][y][z-1] * (-1);
            }
            if(cube_not_empty(x,y,z+1)){
                zfaces[x][y][z+1] = voxel[x][y][z+1] * (1);
            }
        }else{
            if(not cube_valid(size,x-1,y,z) or voxel[x-1][y][z] == -1){
                xfaces[x][y][z] = item * (1);
            }else{
                xfaces[x][y][z] = 0;
            }
            
            if(not cube_valid(size,x+1,y,z) or voxel[x+1][y][z] == -1){
                xfaces[x+1][y][z] = item * (-1);
            }else{
                xfaces[x+1][y][z] = 0;
            }

            if(not cube_valid(size,x,y-1,z) or voxel[x][y-1][z] == -1){
                yfaces[x][y][z] = item * (-1);
            }else{
                yfaces[x][y][z] = 0;
            }

            if(not cube_valid(size,x,y+1,z) or voxel[x][y+1][z] == -1){
                yfaces[x][y+1][z] = item * (1);
            }else{
                yfaces[x][y+1][z] = 0;
            }

            if(not cube_valid(size,x,y,z-1) or voxel[x][y][z-1] == -1){
                zfaces[x][y][z] = item * (1);
            }else{
                zfaces[x][y][z] = 0;
            }

            if(not cube_valid(size,x,y,z+1) or voxel[x][y][z+1] == -1){
                zfaces[x][y][z+1] = item * (-1);
            }else{
                zfaces[x][y][z+1] = 0;
            }
        }
    }

    string obj(){
        auto polygonGrid = PolygonGrid(size);
        auto colorGrid = ColorGrid(color_size);
        string out_faces = "";

        polygonGrid.dim = 0;
        for(int k = 0;k<size+1;k++){
            Array<int> face = Array<int>(xfaces[k][_][_]).reshape(size,size);
            
            /*for(int k = 0; k< face.arrayRef->data_len;k++){
                std::cout << " " << face.arrayRef->data_array[k];
            }*/

            polygonGrid.dim_idx = k;
            auto tmp = splitt_face(face);
            Array<int> lsg_left = tmp[0];
            Array<int> lsg_up = tmp[1];
            Array<int> is_end = tmp[2];

            auto tmp2 = get_idx(lsg_left,lsg_up,is_end,face);
            Array<int> points = tmp2[0];
            Array<int> color = tmp2[1];
            Array<bool> clockwise = tmp2[2];
            

            out_faces +=  create_faces(polygonGrid,colorGrid,points,color,clockwise);
        }
        
        polygonGrid.dim = 1;
        for(int k = 0;k<size+1;k++){
            Array<int> face = Array<int>(yfaces[_][k][_]).reshape(size,size);
            polygonGrid.dim_idx = k;
            auto tmp = splitt_face(face);
            Array<int> lsg_left = tmp[0];
            Array<int> lsg_up = tmp[1];
            Array<int> is_end = tmp[2];

            auto tmp2 = get_idx(lsg_left,lsg_up,is_end,face);
            Array<int> points = tmp2[0];
            Array<int> color = tmp2[1];
            Array<bool> clockwise = tmp2[2];
            out_faces += create_faces(polygonGrid,colorGrid,points,color,clockwise);

        }

        polygonGrid.dim = 2;
        for(int k = 0;k<size+1;k++){
            Array<int> face = Array<int>(zfaces[_][_][k]).reshape(size,size);
            polygonGrid.dim_idx = k;
            auto tmp = splitt_face(face);
            Array<int> lsg_left = tmp[0];
            Array<int> lsg_up = tmp[1];
            Array<int> is_end = tmp[2];

            auto tmp2 = get_idx(lsg_left,lsg_up,is_end,face);
            Array<int> points = tmp2[0];
            Array<int> color = tmp2[1];
            Array<bool> clockwise = tmp2[2];
            out_faces += create_faces(polygonGrid,colorGrid,points,color,clockwise);
        }
        string out = "";
        out += polygonGrid.out;
        out += colorGrid.out;
        out += "vn 1.000000 0.000000 0.000000\n";
        out += "vn -1.000000 0.000000 0.000000\n";
        out += "vn 0.000000 1.000000 0.000000\n";
        out += "vn 0.000000 -1.000000 0.000000\n";
        out += "vn 0.000000 0.000000 1.000000\n";
        out += "vn 0.000000 0.000000 -1.000000\n";
        out += out_faces;
        return out;
    }

};



/*

If you would like to return a copy of NodeContainer, you must implement a copy constructor and assignment operator for it. If you're using a C++11 conformant compiler it will also be good to also implement a move constructor and move assignment operator as well.

On the other hand, if you would like to not create a copy, you must either return a pointer or a reference to the member. You could also make the member a std::shared_ptr, which you may return in this case.

However, in your current implementation you're actually returning a shallow copy of NodeContainer. Once your copy goes out of scope its destructor is called, which deallocates its memory, which in this case is the original memory of your member, effectively making your member invalid. The implementation is not good as it is. So, depending on your goal, either implement the first advised solution, or the second.
*/