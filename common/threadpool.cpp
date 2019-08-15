/*
 * Copyright (C) 2019, Bruno Keymolen (bruno.keymolen@gmail.com)
 */
#include "threadpool.hpp"

namespace keymolen 
{

  //
  //One thread
  //
  ThreadPool::ThreadPoolThread::ThreadPoolThread() : run_(false)
  {
  }

  ThreadPool::ThreadPoolThread::~ThreadPoolThread()
  {
  } 
  
  bool ThreadPool::ThreadPoolThread::run()
  {
    run_ = true;
    thread_ = std::thread(&ThreadPool::ThreadPoolThread::threadloop, this);
    return true;
  }
  
  bool ThreadPool::ThreadPoolThread::stop()
  {
    run_ = false;
    execute([=](void* u1, void* u2){}, NULL, NULL); 
    thread_.join(); 
    return true;
  }
  
  bool ThreadPool::ThreadPoolThread::execute(
      std::function<void (void *usr1, void* usr2)> function,
      void* usr1,
      void* usr2)
  {
    if (pipe.is_empty())
    {
      ThreadPoolFunc f;
      f.func = function;
      f.usr1 = usr1;
      f.usr2 = usr2;
      return pipe.push(f);
    }
    return false;
  }

  void ThreadPool::ThreadPoolThread::threadloop()
  {
    run_ = true;
    while(run_)
    {
      ThreadPool::ThreadPoolFunc f = pipe.peek(true);
      f.func(f.usr1, f.usr2);
      pipe.pull();
    }
  }

  //
  // Pool
  //
  ThreadPool::ThreadPool(int threadcnt) : threadcnt_(threadcnt)
  {
    threads.reserve(threadcnt_);
    for (int i=0; i<threadcnt_; i++)
    {
      ThreadPoolThread *tp = new ThreadPoolThread();
      if (!tp->run())
      {
        LOG_ERR("Could not start thread " << i);
        exit(-1);
      }
      threads.push_back(tp);
    }
  }

  ThreadPool::~ThreadPool()
  {
    for (auto t : threads)
    {
      t->stop();
    }
  }

  bool ThreadPool::ecexute(std::function<void (void *usr1, void* usr2)> function, void *usr1,  void *usr2)
  {
    for (auto t : threads)
    {
      if (t->execute(function, usr1, usr2))
      {
        LOG_DBG("executed on thread : " << LOG_PTR(t));
        return true;
      }
    }
    return false;
  }
 

   
} //ns
