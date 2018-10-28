#include <chrono>
#include <iostream>
#include <list>
#include <random>
#include <thread>

class Object {
public:
    static Object* createRandomly()
    {
        std::random_device seed_gen;
        std::default_random_engine engine(seed_gen());
        std::uniform_int_distribution<> dist(0, 99999);
        auto rand_id = dist(engine);
        return new Object(rand_id);
    }

    explicit Object(int id)
        : _id(id)
    {
    }

    virtual ~Object() {}

    int getId() { return _id; }

protected:
    int _id;
};

const int kThreadNum = 2; // ここの数字を変えてみよう
const int kCount = 100000;

int main()
{
    auto subThreadWork = []() {
        int count = kCount / kThreadNum;
        for (int i = 0; i < count; i++) {
            auto obj = Object::createRandomly();
        }
    };

    auto mainThreadWork = []() {
        int count = kCount % kThreadNum;
        for (int i = 0; i < count; i++) {
            auto obj = Object::createRandomly();
        }
    };

    auto start = std::chrono::system_clock::now();
    std::cout << "計測開始!"
              << "  スレッド数:" << kThreadNum << std::endl;

    // スレッドを生成し、仕事をさせる.
    std::list<std::thread> threads;
    for (int i = 0; i < kThreadNum; i++) {
        threads.push_back(std::thread(subThreadWork));
    }
    for (auto& t : threads) {
        // 対象のスレッドの仕事が終わるまで待つ.
        t.join();
    }

    // 余り分は、メインスレッドで仕事する.
    mainThreadWork();

    auto end = std::chrono::system_clock::now();
    auto dur = end - start;
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    std::cout << "計測終了!"
              << "  経過時間(msec):" << msec << std::endl;

    return 0;
}
