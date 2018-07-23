#include <vector>
#include <underline.h>

void func() {
    _::map(std::vector<int>{1,2,3}, [](int item, std::string index) {
    });
}
