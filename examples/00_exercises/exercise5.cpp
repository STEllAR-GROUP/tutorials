#include <hpx/hpx_main.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/lcos.hpp>

// This program does not compile. Try to fix it.

int main()
{
    auto f = hpx::make_ready_future<int>(3);
    hpx::cout << f << hpx::endl;

    return 0;
}
