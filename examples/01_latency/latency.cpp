//  Copyright (c) 2013-2015 Hartmut Kaiser
//  Copyright (c) 2013 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Bidirectional network bandwidth test

#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/serialization.hpp>
#include <hpx/include/parallel_for_each.hpp>
#include <hpx/lcos/local/sliding_semaphore.hpp>
#include <hpx/parallel/executors/static_chunk_size.hpp>
//
#include <boost/scoped_array.hpp>
//
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <mutex>
//
#ifdef DDT_MPI_INIT
 #include "mpi.h"
#endif

// ---------------------------------------------------------------------------------
#define SKIP_LARGE  100

// ---------------------------------------------------------------------------------
char* align_buffer (char* ptr, unsigned long align_size)
{
    return (char*)(((std::size_t)ptr + (align_size - 1)) / align_size * align_size);
}

#if defined(HPX_WINDOWS)
unsigned long getpagesize()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize;
}
#endif

// ---------------------------------------------------------------------------------
hpx::serialization::serialize_buffer<char>
message(hpx::serialization::serialize_buffer<char> const& receive_buffer)
{
    return receive_buffer;
}

// this declares an action called message_action
HPX_PLAIN_DIRECT_ACTION(message);

HPX_REGISTER_BASE_LCO_WITH_VALUE_DECLARATION(
    hpx::serialization::serialize_buffer<char>, serialization_buffer_char);
HPX_REGISTER_BASE_LCO_WITH_VALUE(
    hpx::serialization::serialize_buffer<char>, serialization_buffer_char);

// ---------------------------------------------------------------------------------
typedef hpx::lcos::local::mutex              mutex_type;
typedef std::unique_lock<mutex_type>         unique_lock;
typedef hpx::lcos::local::condition_variable condition_var_type;

// ---------------------------------------------------------------------------------
// Send a message and receives the reply using a vector<future> to track
// messages in flight. Guaranteed to wait correctly after each batch of
// 'window_size' messages, but needs more effort to maintain 'window_size' messages
// in flight all the time.
double receive_v1(
    hpx::naming::id_type dest,
    char * send_buffer,
    std::size_t size,
    std::size_t loop,
    std::size_t window_size)
{
    typedef hpx::serialization::serialize_buffer<char> buffer_type;
    buffer_type recv_buffer;

    message_action            msg;
    std::atomic<unsigned int> counter;
    // vector to store returns in
    std::vector<hpx::future<hpx::serialization::serialize_buffer<char>>> messages;
    messages.reserve(window_size);
    //
    std::size_t skip = SKIP_LARGE;
    std::size_t parcel_count = 0;
    
    // warm up
    for (int s=0; s<skip; ++s) {
        msg(dest,buffer_type(send_buffer, size, buffer_type::reference));
    }
    //
    hpx::util::high_resolution_timer t;
    //    
    for (std::size_t i = 0; i<loop; ++i) {
        for (std::size_t j = 0; j<window_size; ++j) {
            // launch a message to the remote node
            messages.push_back(hpx::async(msg, dest,
                buffer_type(send_buffer, size, buffer_type::reference)));
            //
            parcel_count++;
        }
        hpx::wait_all(messages);
        messages.clear();
    }    
    double d = (static_cast<double>(parcel_count));
    double elapsed = t.elapsed();
    return 0.5*(elapsed * 1e6) / d;    
}

// ---------------------------------------------------------------------------------
// Send a message and receives the reply with a simple
// atomic counter being used to check how many messages have been returned
// the last message to return wakes up a condition variable that was waiting
// in the main task
double receive_v2(
    hpx::naming::id_type dest,
    char * send_buffer,
    std::size_t size,
    std::size_t loop,
    std::size_t window_size)
{
    std::size_t skip = SKIP_LARGE;

    typedef hpx::serialization::serialize_buffer<char> buffer_type;
    buffer_type recv_buffer;

    message_action            msg;
    condition_var_type        cv;
    mutex_type                mutex_;
    std::atomic<unsigned int> counter;

    // warm up
    for (int s=0; s<skip; ++s) {
        msg(dest,buffer_type(send_buffer, size, buffer_type::reference));
    }
    //
    hpx::util::high_resolution_timer t;
    //
    for (std::size_t i = 0; i < loop; ++i)
    {
        counter = 0;
        //
        for (std::size_t i=0; i< window_size; ++i) {
            hpx::async(
                msg, dest, buffer_type(send_buffer, size, buffer_type::reference)).then(
                [&](auto && f){
                    if (++counter == window_size) {
                        cv.notify_one();
                    }
                }
            );
        }
        // wait until all are done
        unique_lock lk(mutex_);
        cv.wait(lk, [&]{return counter==window_size;});
    }

    double elapsed = t.elapsed();
    return (elapsed * 1e6) / (2 * loop * window_size);
}

// ---------------------------------------------------------------------------------
// Send a message and receives the reply using a sliding_semaphore to
// track messages in flight. There are always 'window_size' messages in transit
// at any time
// Warning : message N might be returned after message N+M because at the remote
// end each message return is triggered on an HPX task which may or may not
// be suspended and delay the current return message.
// This means that when message N completes- we cannot be 100% that 'window_size'
// messages are really in flight, but we get close. Also when the loop terminates
// there may be one or more messages still uncompleted, so we wait for them at the end
// to avoid destroying the CV before it is done with
double receive_v3(
    hpx::naming::id_type dest,
    char * send_buffer,
    std::size_t size,
    std::size_t loop,
    std::size_t window_size)
{
    typedef hpx::serialization::serialize_buffer<char> buffer_type;
    buffer_type recv_buffer;

    message_action            msg;
    condition_var_type        cv;
    mutex_type                mutex_;
    std::atomic<unsigned int> counter;
    // we want N messages in flight at once, so we must wait

    hpx::lcos::local::sliding_semaphore sem(window_size-1, -1);
    //
    std::size_t skip = SKIP_LARGE;
    std::size_t parcel_count = 0;
    counter = 0;

    // warm up
    for (int s=0; s<skip; ++s) {
        msg(dest,buffer_type(send_buffer, size, buffer_type::reference));
    }
    //
    hpx::util::high_resolution_timer t;
    //
    for (std::size_t i = 0; i < (loop*window_size); ++i) {
        // launch a message to the remote node
        hpx::async(msg, dest,
            buffer_type(send_buffer, size, buffer_type::reference)).then(
                hpx::launch::sync,
                [&,parcel_count](auto &&f) -> void {
                    // when the message completes, increment our semaphore count
                    // so that N are always in flight
                    sem.signal(parcel_count);
                    //
                    if (++counter == (loop*window_size)) {
                        cv.notify_one();
                    }
                }
            );

        //
        sem.wait(parcel_count);
        //
        parcel_count++;
    }
    unique_lock lk(mutex_);
    cv.wait(lk, [&]{return counter == (loop*window_size);});
    //
    double d = (static_cast<double>(parcel_count));
    double elapsed = t.elapsed();
    return 0.5*(elapsed * 1e6) / d;
}

// ---------------------------------------------------------------------------------
void print_header(const std::string &method)
{
    hpx::cout << "# Latency Test : " << method << "\n"
              << "# Size    Latency (microsec)"
              << std::endl;
}

// ---------------------------------------------------------------------------------
void run_benchmark(boost::program_options::variables_map & vm)
{
    // use the first remote locality to bounce messages, if possible
    hpx::id_type here = hpx::find_here();
    hpx::id_type there = here;
    std::vector<hpx::id_type> localities = hpx::find_remote_localities();
    if (!localities.empty())
        there = localities[0];

    std::size_t window_size = vm["window-size"].as<std::size_t>();
    std::size_t loop = vm["loop"].as<std::size_t>();
    std::size_t min_size = vm["min-size"].as<std::size_t>();
    std::size_t max_size = vm["max-size"].as<std::size_t>();

    if(max_size < min_size) std::swap(max_size, min_size);

    // align used buffers on page boundaries
    unsigned long align_size = getpagesize();
    boost::scoped_array<char> send_buffer_orig(new char[max_size + align_size]);
    char* send_buffer = align_buffer(send_buffer_orig.get(), align_size);

    // perform actual measurements
    print_header("Vector of futures");
    hpx::util::high_resolution_timer timer;
    for (std::size_t size = min_size; size <= max_size; size *= 2)
    {
        double latency = receive_v1(there, send_buffer, size, loop, window_size);
        hpx::cout << std::left << std::setw(10) << size
                  << latency << hpx::endl << hpx::flush;
    }
    hpx::cout << "Total time: " << timer.elapsed_nanoseconds() << std::endl;
    
    print_header("Atomic counter");    
    timer.restart();
    for (std::size_t size = min_size; size <= max_size; size *= 2)
    {
        double latency = receive_v2(there, send_buffer, size, loop, window_size);
        hpx::cout << std::left << std::setw(10) << size
                  << latency << hpx::endl << hpx::flush;
    }
    hpx::cout << "Total time: " << timer.elapsed_nanoseconds() << std::endl;
    
    print_header("Sliding semaphore");
    timer.restart();
    for (std::size_t size = min_size; size <= max_size; size *= 2)
    {
        double latency = receive_v3(there, send_buffer, size, loop, window_size);
        hpx::cout << std::left << std::setw(10) << size
                  << latency << hpx::endl << hpx::flush;
    }


}

// ---------------------------------------------------------------------------------
int hpx_main(boost::program_options::variables_map & vm)
{
    run_benchmark(vm);
    return hpx::finalize();
}

// ---------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
#ifdef DDT_MPI_INIT
    MPI_Init(&argc, &argv);
#endif

    boost::program_options::options_description
        desc("Usage: " HPX_APPLICATION_STRING " [options]");

    desc.add_options()
        ("window-size",
         boost::program_options::value<std::size_t>()->default_value(1),
         "Number of messages to send in parallel")
        ("loop",
         boost::program_options::value<std::size_t>()->default_value(100),
         "Number of loops")
        ("min-size",
         boost::program_options::value<std::size_t>()->default_value(1),
         "Minimum size of message to send")
        ("max-size",
         boost::program_options::value<std::size_t>()->default_value((1<<22)),
         "Maximum size of message to send");

    return hpx::init(desc, argc, argv);
}

