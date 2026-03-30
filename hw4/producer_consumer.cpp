// Consumer Producer Template in C++ using semaphores.
// Must compile with C++20

#include <semaphore>
#include <thread>
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <ctime>

using namespace std;

const int buffer_size = 6;        // size of buffer
int buffer[buffer_size];          // buffer holds 6 cells 0-5

const int max_iterations = 30;    // stop after 30 items

int in_ptr = 0;
int out_ptr = 0;

// Semaphores
counting_semaphore<buffer_size> sem_empty(buffer_size); // initially all empty
counting_semaphore<buffer_size> sem_full(0);            // initially none full

void Producer(int id)
{
    for (int item = 1; item <= max_iterations; item++)
    {
        sem_empty.acquire();      // wait if buffer full

        buffer[in_ptr] = item;

        cout << "  Producer " << id
             << " entered: " << buffer[in_ptr]
             << "  in cell: " << in_ptr
             << endl;

        in_ptr = (in_ptr + 1) % buffer_size;

        sem_full.release();       // signal that item is available

        this_thread::sleep_for(
            chrono::milliseconds(rand() % 1000 + 1000));
    }
}

void Consumer(int id)
{
    for (int item = 1; item <= max_iterations; item++)
    {
        sem_full.acquire();       // wait if buffer empty

        cout << "                                        Consumer " << id
             << " consumed: " << buffer[out_ptr]
             << "  from cell: " << out_ptr
             << endl;

        out_ptr = (out_ptr + 1) % buffer_size;

        sem_empty.release();      // signal empty slot available

        this_thread::sleep_for(
            chrono::milliseconds(rand() % 500 + 1500));
    }
}

int main()
{
    srand(time(NULL));

    auto start = chrono::steady_clock::now();

    thread producer(Producer, 1);
    thread consumer(Consumer, 1);

    producer.join();
    consumer.join();

    auto stop = chrono::steady_clock::now();

    cout << "\n  Time (ms): "
         << chrono::duration_cast<chrono::milliseconds>(stop - start).count()
         << "\n";

    return 0;
}

// How To Run:
// g++ -std=c++20 producer_consumer.cpp -pthread -o producer_consumer
// ./producer_consumer
