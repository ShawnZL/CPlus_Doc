#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <unistd.h>

std::mutex  mtx_syn;
std::condition_variable cv_syn;
std::condition_variable cv_syn_1;
bool ready = false;
void threadA (int id) {
    while (1)
    {
        std::unique_lock<std::mutex> lck(mtx_syn);
        while (!ready)
        {
            cv_syn.wait(lck);
        }

        std::cout << "threadA " << id << '\n';
        usleep(500*1000);
        cv_syn.notify_one();   // 唤醒等待线程中的一个
        cv_syn.wait(lck);      // 等待
    }
}

void threadB (int id) {
    while (1)
    {
        //新创建的 unique_lock 对象管理 Mutex 对象 m，并尝试调用 m.lock() 对 Mutex 对象进行上锁，如果此时另外某个 unique_lock 对象已经管理了该 Mutex 对象 m，则当前线程将会被阻塞
        std::unique_lock<std::mutex> lck(mtx_syn);
        while (!ready)
        {
            cv_syn.wait(lck);
        }
        // ...
        std::cout << "threadB " << id << '\n';
        usleep(500*1000);
        cv_syn.notify_one();// 唤醒等待线程中的一个
        cv_syn.wait(lck);
    }
}

void threadC(int id)
{
    while (1)
    {
        std::unique_lock<std::mutex> lck(mtx_syn);
        while (!ready) cv_syn.wait(lck);
        // ...
        std::cout << "threadC " << id << '\n';
        usleep(500*1000);
        cv_syn.notify_one();  // 唤醒等待线程中的一个线程
        cv_syn.wait(lck);
    }
}


void hello() {
    std::unique_lock<std::mutex> lck(mtx_syn);
    ready = true;
    //cv_syn.notify_one(); // 唤醒等待线程中的一个线程，notify_one()唤醒的线程顺序执行
    cv_syn.notify_all();  //唤醒的线程抢占式知性
}

int main() {
    std::thread threads[5];
    threads[0] = std::thread(threadA, 0);
    threads[1] = std::thread(threadB, 1);
    threads[2] = std::thread(threadC, 2);
    std::cout << "3 threads ready to race...\n";
//    hello();                       // go!
    ready = true;
    for (auto& th: threads) th.join();
    std::cout << "done" << std::endl;
    return 0;
}
