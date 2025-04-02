#include <iostream>
#include <string>
// this header declares std::memcpy
#include <cstring>

using namespace std;

// '{}' means list initialization or brace initialization (from C++11)
// if you want to use C++98 initialization style, you can define:
// const std::string secret = "...";
// const std::string secret("...");
const std::string secret {
    "Ifmmp-!xpsme\"\012J(n!tpssz-!Ebwf/!"
    "J(n!bgsbje!J!dbo(u!ep!uibu/!.!IBM\01"};

const auto sz = secret.size();

int main(){
    // allocate memory on heap
    char* result = new char[sz];

    std::memcpy(result, secret.data(),  sz);

    for(int i=0; i<sz; i++) {
        result[i] -= 1;
    }

    std::cout << result << "\n";
    // remember to release allocated memory
    delete[] result;
    return 0;
}
