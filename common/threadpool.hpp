/*
 * Copyright (C) 2019, Bruno Keymolen (bruno.keymolen@gmail.com)
 */

#ifndef COMMON_THREADPOOL
#define COMMON_THREADPOOL

#include <vector>
#include <thread>
#include <functional>

#include "common.hpp"
#include "async_pipe.hpp"


namespace keymolen 
{

    class ThreadPool
    {
      public:
        struct ThreadPoolFunc
        {
          void* usr1;
          void* usr2;
          std::function<void (void *usr1, void* usr2)> func;
        };
        class ThreadPoolThread
        {
          public:
            ThreadPoolThread();
            virtual ~ThreadPoolThread();
            AsyncPipe<ThreadPoolFunc> pipe;
            bool run();
            bool stop();
            bool execute(std::function<void (void *usr1, void* usr2)> f, void* usr1, void* usr2);     private:            
            void threadloop();
            std::thread thread_;
            bool run_;
        };
      public:
        ThreadPool(int threadcnt);
        virtual ~ThreadPool();
        bool ecexute(std::function<void (void *usr1, void* usr2)> function, void *usr1,  void *usr2);
      private:
        int threadcnt_;
        std::vector<ThreadPoolThread*> threads;
    };

} //ns
    


#endif
