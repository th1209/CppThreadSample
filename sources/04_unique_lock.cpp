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

    // TODO:
    // mutexを外部に渡す際、こんな感じでポインタ渡しするでいいのだろうか?
    // ベストプラクティスが知りたい.
    std::mutex* getMutex()
    {
        return &_mutex;
    }

    void addCount()
    {
        ++_counter;
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
    AtomicCounter ac1;
    AtomicCounter ac2;

    auto work = [](AtomicCounter& ac1, AtomicCounter& ac2) {
        for (int i = 0; i < 5; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // unique_lockの特徴1: 複数リソースのロックが可能
            std::unique_lock<std::mutex> ul1(*(ac1.getMutex()), std::defer_lock);
            std::unique_lock<std::mutex> ul2(*(ac2.getMutex()), std::defer_lock);

            // unique_lockの特徴2: defer_lockとlock()を利用した、任意のタイミングでの遅延ロックが可能
            std::lock(ul1, ul2);

            ac1.addCount();
            ac2.addCount();
        }
    };

    std::vector<std::thread> threads(4);
    for (std::thread& thread : threads) {
        thread = std::thread(work, std::ref(ac1), std::ref(ac2));
    }
    for (std::thread& thread : threads) {
        thread.join();
    }

    std::cout << "ac1: " << ac1.getCount() << std::endl;
    std::cout << "ac2: " << ac2.getCount() << std::endl;

    return 0;
}
