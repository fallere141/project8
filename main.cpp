#include <iostream>
#include <string>


void id_increase(int &id) {
    id = 5;
}


int main() {
    int id = 0;
    id_increase(id);
    std::cout << id << std::endl;
    std::string buffer_in="dw";
    std::string buffer_out="dw";
    buffer_out.append("//");
    buffer_out.append("\n");
    std::cout<<buffer_out<<"ok";
    return 0;
}
