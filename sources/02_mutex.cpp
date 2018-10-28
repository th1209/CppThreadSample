#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

class AtomicCounter {
public:
    AtomicCounter()
        : _counter(0)
    {
    }

    ~AtomicCounter() {}

    void addCount()
    {
        // ↓ lock() ~ unlock() までの処理が、クリティカルセクションとして扱われる.
        _mutex.lock();
        ++_counter;
        _mutex.unlock();
    }

    int getCount()
    {
        return _counter;
    }

private:
    std::mutex _mutex;
    int _counter;
};

int main()
{
    AtomicCounter ac;

    auto work = [](AtomicCounter& ac) {
        for (int i = 0; i < 5; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ac.addCount();
        }
    };

    std::vector<std::thread> threads(4);
    for (std::thread& thread : threads) {
        thread = std::thread(work, std::ref(ac));
    }
    for (std::thread& thread : threads) {
        thread.join();
    }

    std::cout << ac.getCount() << std::endl;

    return 0;
}
