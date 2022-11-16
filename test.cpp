#include <iostream>
#include <thread>
#include <pthread.h>
#include <chrono>
#include <vector>


void work() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "Do work...\n";
    std::cout << "Thread id " << std::this_thread::get_id() << '\n';
}

int main()
{
    std::vector<std::thread> th;
    for (int i = 0; i < 10; i++) {
         th.emplace_back(work);

    }
    std::cout << "Main thread id " << std::this_thread::get_id() << '\n';
    for (int i = 0; i < 10; i++) {
        th[i].join();
    }
    return 0;
};