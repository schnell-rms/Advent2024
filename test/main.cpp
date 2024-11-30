#include <iostream>
#include <sstream>
#include <fstream>

#include <utils.h>
#include <string>
#include <unordered_map>

#include <unordered_set>
#include <iterator>

using namespace std;

void foo(int a) {
    cout << "foo: " << a << endl;
}

void bar(int a) {
    cout << "bar: " << a << endl;
}

int main(int argc, char *argv[]) {

    foo(7);
    bar(9);
    return EXIT_SUCCESS;
}
