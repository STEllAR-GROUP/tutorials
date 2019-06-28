#include <hpx/hpx_main.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/iostreams.hpp>

// This program does not compile. Try to fix it.

void fun(double x)
{
    hpx::cout << "The value of x is " << x << hpx::endl;
}

int main()
{
    auto f = hpx::async([](){ return 3.14; });
    f.then(&fun);

    return 0;
}
