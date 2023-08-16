# join wait

![](https://github.com/ShawnZL/CPlus_Doc/raw/master/pic/join_wait.png)

`join` 保障执行顺序

`wait` 等待。需要其它的接口来唤醒，变为阻塞状态。

**同步队列（锁池）**：假设线程A已经拥有了某个对象（注意:不是类）的锁，而其它的线程想要调用这个对象的某个synchronized方法(或者synchronized块)，由于这些线程在进入对象的synchronized方法之前必须先获得该对象的锁的拥有权，但是该对象的锁目前正被线程A拥有，所以这些线程就进入了该对象的同步队列（锁池）中，这些线程状态为Blocked。**当线程试图执行某个操作（如访问共享资源、获取互斥锁等）时，如果该操作无法立即完成，线程可能会被阻塞。被阻塞的线程被称为"blocked"，它暂时无法继续执行。这种情况通常是因为线程在等待某个条件满足，或者在等待某个事件发生。**

**等待队列（等待池）**：假设一个线程A调用了某个对象的wait()方法，线程A就会释放该对象的锁（因为wait()方法必须出现在synchronized中，这样自然在执行wait()方法之前线程A就已经拥有了该对象的锁），同时线程A就进入到了该对象的等待队列（等待池）中，此时线程A状态为Waiting。如果另外的一个线程调用了相同对象的notifyAll()方法，那么 处于该对象的等待池中的线程就会全部进入该对象的同步队列（锁池）中，准备争夺锁的拥有权。如果另外的一个线程调用了相同对象的notify()方法，那么 仅仅有一个处于该对象的等待池中的线程（随机）会进入该对象的同步队列（锁池）。**当线程主动调用某些线程同步机制（如条件变量的 `wait` 操作、信号量的 `wait` 操作等）时，线程会进入"waiting"状态。在这种状态下，线程主动放弃了CPU的执行权，直到某个条件被满足，线程被唤醒并继续执行。**

```c
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
    hello();                       // go!
    for (auto& th: threads) th.join();
    std::cout << "done" << std::endl;
    return 0;
}
```

