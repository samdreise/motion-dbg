#include <iostream>
#include <vector>

int main() {
    int a = 0;
    int b = 0;
    std::vector<int> vec{};
    vec.push_back(a);
    vec.push_back(b);

    std::cout << "Hello World!\n";



    for (int i = 0; i < 5; ++i) {
        vec.push_back(i + a);
        b += vec[i];
        int x = 0;
        x = b;
        vec[i] = x;
    }


    a = 4;
    b = 3;

    std::cout << 4 + 3 << std::endl;

    return 0;

}
