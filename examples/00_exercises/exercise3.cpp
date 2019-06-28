#include <hpx/hpx_main.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/util.hpp>

// This program does not compile. Try to fix it. There are multiple correct
// solutions.

void fun(hpx::future<double> f, hpx::future<int> g)
{
    hpx::cout << "The value of f is " << f.get();
    hpx::cout << "The value of g is " << g.get();
}

int main()
{
    auto f = hpx::async([]() -> double { std::cout << "1\n"; return 3.14; });
    auto g = hpx::async([]() -> int { std::cout << "2\n"; return 42; });

    hpx::when_all(f, g).then(&fun);

    return 0;
}
