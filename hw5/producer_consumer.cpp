// Homework #5
// Multiple Producer / Consumer (3 Producers, 2 Consumers)
// Produces numbers 1–20 in order
// Prints total execution time at the end

#include <semaphore>
#include <thread>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>

using namespace std;

const int buffer_size = 6;
int buffer[buffer_size];

const int max_iterations = 20;

// shared state
int in_ptr = 0;
int out_ptr = 0;

int nextProduced = 1;
int nextConsumed = 1;

int produced_count = 0;
int consumed_count = 0;

// semaphores
counting_semaphore<buffer_size> sem_empty(buffer_size);
counting_semaphore<buffer_size> sem_full(0);
binary_semaphore sem_mutex(1);

void Producer(int id)
{
    while (true)
    {
        sem_empty.acquire();
        sem_mutex.acquire();

        if (produced_count >= max_iterations)
        {
            sem_mutex.release();
            sem_empty.release();
            break;
        }

        int item = nextProduced++;
        buffer[in_ptr] = item;

        cout << "Producer_" << id
             << " entered: " << item
             << "  into cell:" << in_ptr
             << endl;

        in_ptr = (in_ptr + 1) % buffer_size;
        produced_count++;

        sem_mutex.release();
        sem_full.release();

        this_thread::sleep_for(
            chrono::milliseconds(rand() % 300 + 200));
    }
}

void Consumer(int id)
{
    while (true)
    {
        sem_full.acquire();
        sem_mutex.acquire();

        if (consumed_count >= max_iterations)
        {
            sem_mutex.release();
            sem_full.release();
            break;
        }

        int item = buffer[out_ptr];

        cout << "                                   Consumer_" << id
             << " consumed: " << item
             << "  in cell: " << out_ptr
             << endl;

        out_ptr = (out_ptr + 1) % buffer_size;
        nextConsumed++;
        consumed_count++;

        sem_mutex.release();
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

    // 3 Producers
    for (int i = 1; i <= 3; i++)
        producers.emplace_back(Producer, i);

    // 2 Consumers
    for (int i = 1; i <= 2; i++)
        consumers.emplace_back(Consumer, i);

    for (auto &p : producers)
        p.join();

    // release consumers if waiting
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
