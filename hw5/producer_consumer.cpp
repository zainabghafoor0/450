// Homework #5
// Multiple Producer / Consumer (3 Producers, 2 Consumers)
// Produces numbers 1–20 in order
// Prints total execution time at the end
// C++20

#include <semaphore>
#include <thread>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <ctime>

using namespace std;

const int buffer_size = 6;
int buffer[buffer_size];

const int max_iterations = 20;

// shared circular buffer pointers
int in_ptr = 0;
int out_ptr = 0;

// counters
int nextProduced = 1;
int nextConsumed = 1;

int produced_count = 0;
int consumed_count = 0;

// semaphores
counting_semaphore<buffer_size> sem_empty(buffer_size);
counting_semaphore<buffer_size> sem_full(0);

// renamed mutex semaphores
binary_semaphore sem_mutex_prod(1);    // protects producer counters
binary_semaphore sem_mutex_cons(1);    // protects consumer counters
binary_semaphore sem_mutex_buffer(1);  // protects shared buffer + in_ptr/out_ptr

void Producer(int id)
{
    while (true)
    {
        // Reserve next item safely
        sem_mutex_prod.acquire();

        if (produced_count >= max_iterations)
        {
            sem_mutex_prod.release();
            break;
        }

        int item = nextProduced;
        nextProduced++;
        produced_count++;

        sem_mutex_prod.release();

        // Wait for empty slot
        sem_empty.acquire();

        // Access shared buffer safely
        sem_mutex_buffer.acquire();

        buffer[in_ptr] = item;

        cout << "Producer_" << id
             << " entered: " << item
             << "  into cell:" << in_ptr
             << endl;

        in_ptr = (in_ptr + 1) % buffer_size;

        sem_mutex_buffer.release();

        // Signal full slot
        sem_full.release();

        this_thread::sleep_for(
            chrono::milliseconds(rand() % 300 + 200));
    }
}

void Consumer(int id)
{
    while (true)
    {
        // Wait for full slot
        sem_full.acquire();

        // Check whether all items are already consumed
        sem_mutex_cons.acquire();
        if (consumed_count >= max_iterations)
        {
            sem_mutex_cons.release();
            sem_full.release();
            break;
        }
        sem_mutex_cons.release();

        // Access shared buffer safely
        sem_mutex_buffer.acquire();

        int item = buffer[out_ptr];

        cout << "                                   Consumer_" << id
             << " consumed: " << item
             << "  in cell: " << out_ptr
             << endl;

        out_ptr = (out_ptr + 1) % buffer_size;

        sem_mutex_buffer.release();

        // Update consumer counters safely
        sem_mutex_cons.acquire();
        nextConsumed++;
        consumed_count++;
        sem_mutex_cons.release();

        // Signal empty slot
        sem_empty.release();

        this_thread::sleep_for(
            chrono::milliseconds(rand() % 400 + 300));
    }
}

int main()
{
    srand(time(NULL));

    auto start = chrono::steady_clock::now();

    vector<thread> producers;
    vector<thread> consumers;

    for (int i = 1; i <= 3; i++)
        producers.emplace_back(Producer, i);

    for (int i = 1; i <= 2; i++)
        consumers.emplace_back(Consumer, i);

    for (auto &p : producers)
        p.join();

    // Wake consumers if blocked
    for (int i = 0; i < 2; i++)
        sem_full.release();

    for (auto &c : consumers)
        c.join();

    auto stop = chrono::steady_clock::now();

    cout << "\nTotal Time (ms): "
         << chrono::duration_cast<chrono::milliseconds>(stop - start).count()
         << endl;

    return 0;
}
