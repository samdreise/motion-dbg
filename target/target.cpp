#include <iostream>
#include <vector>

struct Coordinates {
    int x;
    int y;

};

struct StructObject {
    bool its_a_bool;
    Coordinates coordinates;
    std::string raw_text;
    std::vector<float> vec_array;

};


int main() {
    int ez_int = 0;
    std::string some_text = "Hello\n";
    StructObject struct_object;

    struct_object.coordinates = { 55, 55 };
    struct_object.raw_text = "StructObject has some raw text! and a \nnewline. Also a \\ backslash! (finally some \"quotation marks\". And thats it!)";
    ez_int = 3;
    struct_object.vec_array.push_back(1.1f);
    struct_object.vec_array.push_back(2.2f);
    some_text += "Again!!\n";
    struct_object.coordinates = { 33, 33 };
    ez_int = -1;
    struct_object.raw_text = "Line 31 changes the text!";

    struct_object.vec_array.push_back(3.3f);
    return 0;

}
