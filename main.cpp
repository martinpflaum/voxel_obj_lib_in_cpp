#include "obj.hpp"


int main(){
    auto worldGrid = WorldGrid(4,8);
    worldGrid.setitem(1,1,1,2);
    std::cout << worldGrid.obj() << "\n";
}