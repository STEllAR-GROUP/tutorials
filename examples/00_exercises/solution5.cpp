#include <hpx/hpx_main.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/lcos.hpp>

int main()
{
    auto f = hpx::make_ready_future<int>(3);
    // HPX iostream errors are very verbose. Look at the first error message.
    hpx::cout << f.get() << hpx::endl;

    return 0;
}
