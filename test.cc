#include <iostream>
#include <thread>

using namespace std;

int main()
{
    thread* t = new thread([&]() {
        while (true)
        {
            cout << "data" << endl;
        }
    });

    // t->detach();
    this_thread::sleep_for(std::chrono::milliseconds(10));
}