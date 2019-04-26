1. Thread.{h, cpp}是对线程原语pthread_create和pthread_join的封装,其中线程的入口函数封装在了ThreadData结构体中。
2. MutexLock.{h, cpp}是对锁的RAII封装,同理Condition.{h, cpp}。
3. CountDownLatch.{h, cpp}是基于锁和条件变量的更高级的并发编程构建,同理BlockingQueue(这里没有实现)。
4. CurrnetThread.h 内容不多,抄的muduo,大致就是获得当前线程的pid。
5. noncopyable.h 通过将拷贝构造和拷贝赋值定义为private来阻止拷贝控制,其余类继承该类从而阻止拷贝。
---
6. 日志库的设计
