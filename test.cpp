#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
#include <future>
#include <thread>
#include <chrono>

void f()
{
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9};

    std::vector<std::pair<int, int>> vp;
    vp.reserve(v.size());
    std::transform(v.begin(), v.end(), std::back_inserter(vp), [&](const auto& It){
        static int i = 0;
        std::pair<int, int> temp(i++, It);
        return (std::move(temp));
    });
    for (auto& x : vp)
    {
        std::cout << x.first << ':' << x.second << ' ';
    }
    std::cout << std::endl;
}

int main()
{
    f();
    f();
}