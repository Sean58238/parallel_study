#include <iostream>
#include <string>

using namespace std;

const std::string secret {
    "Ifmmp-!xpsme\"\012J(n!tpssz-!Ebwf/!"
    "J(n!bgsbje!J!dbo(u!ep!uibu/!.!IBM\01"};

const auto sz = secret.size();

int main(){

    std::memcpy(result, secret.data(),  sz);

    for(int i=0; i++; i<sz) {
        result[i] -= 1;
    }

    std::cout << result << "\n";
    return 0;
}