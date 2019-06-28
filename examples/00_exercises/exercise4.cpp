#include <hpx/hpx_main.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/util.hpp>

#include <cstddef>

// This program does not compile. Try to fix it. There are multiple correct
// solutions.

void fun(hpx::future<std::vector<std::size_t>> f)
{
    hpx::cout << "This week's lucky numbers are:";

    auto v = f.get();
    for (auto& x : v)
    {
        hpx::cout << " " << x;
    }

    hpx::cout << hpx::endl;
}

int main()
{
    std::vector<hpx::future<std::size_t>> v;
    for (std::size_t i = 0; i < 7; ++i)
    {
        v.push_back(hpx::async([i]() { return i; }));
    }

    hpx::when_all(v).then(&fun);

    return 0;
}
