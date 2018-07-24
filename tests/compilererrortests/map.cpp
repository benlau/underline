#include <vector>
#include <underline.h>

int main() {
    _::map(std::vector<int>{1,2,3}, [](int item, std::string index) {
    });

    _::map(123, [](){});
}

