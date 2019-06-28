#include <hpx/hpx_main.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/util.hpp>

void fun(hpx::future<hpx::util::tuple<hpx::future<double>, hpx::future<int>>> f)
{
    auto t = f.get();
    hpx::cout
        << "The value of f is "
        << hpx::util::get<0>(t).get() << hpx::endl;
    hpx::cout
        << "The value of g is "
        << hpx::util::get<1>(t).get() << hpx::endl;
}

int main()
{
    auto f = hpx::async([]() -> double { std::cout << "1\n"; return 3.14; });
    auto g = hpx::async([]() -> int { std::cout << "2\n"; return 42; });

    // when_all produces a future<tuple<future<double>, future<int>>>. dataflow
    // can also be used to avoid having to take a tuple as an argument in fun.
    hpx::when_all(f, g).then(&fun);

    return 0;
}
