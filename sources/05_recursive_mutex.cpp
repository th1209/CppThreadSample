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
        std::lock_guard<std::recursive_mutex> lock(_rec_mutex);
        ++_counter;
    }

    void setCount(int count)
    {
        // recursive_mutexを使うと、同スレッドによるデッドロックを防げる.
        // 多少のオーバーヘッドはあるものの、原則としてはこちらを使った方が良いだろう.
        // recursive_mutexについては、以下の素晴らしいエントリを参照させていただいた.
        // https://qiita.com/BugDig/items/80364b4e1c28b1685ab1

        std::lock_guard<std::recursive_mutex> lock(_rec_mutex);
        addCount(); // ここでワザとらしくaddCountを呼び、デッドロックの発生を試みる
        _counter = count;
    }

    int getCount()
    {
        std::lock_guard<std::recursive_mutex> lock(_rec_mutex);
        return _counter;
    }

private:
    std::recursive_mutex _rec_mutex;
    int _counter;
};

int main()
{
    AtomicCounter ac;

    auto work = [](AtomicCounter& ac) {
        for (int i = 0; i < 5; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ac.setCount(100);
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

