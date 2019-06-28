#include <hpx/hpx_main.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/iostreams.hpp>

// This program compiles, but fails at runtime. Try to fix it.

int main()
{
    auto f = hpx::async([](){});

    f.then([](hpx::future<void>) { hpx::cout << "Hello from first lambda" << hpx::endl; });
    f.then([](hpx::future<void>) { hpx::cout << "Hello from second lambda" << hpx::endl; });

    return 0;
}
