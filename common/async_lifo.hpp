/*
 * Copyright (C) 2019, Bruno Keymolen (bruno.keymolen@gmail.com)
 */

//
// -push-> | size | -drop->
// <-pull- |      |
//

#ifndef COMMON_ASYNC_LIFO_HPP
#define COMMON_ASYNC_LIFO_HPP

#include "common.hpp"

#include <mutex>
#include <deque>
#include <functional>
#include <condition_variable>


namespace keymolen {

    template <typename T> class AsyncLifo 
    {
    public:
        AsyncLifo(unsigned int max_elements,  std::function<void (T& t)> drop_cb = nullptr) : max_elements_(max_elements), drop_cb_(drop_cb)
        {

        }
        void push(T& t)
        {
            mutex_.lock();
            buffer_.push_front(t);
            while (buffer_.size() > max_elements_)
            {
              if (drop_cb_ != nullptr)
              {
                T& td = buffer_.front();
                drop_cb_(td);
              }
              buffer_.pop_back();
            }
            mutex_.unlock();
            condition_var_.notify_one();
        }
        T pull(bool blocking=false)
        {
            return get_element(blocking, true);
        }
        T peek(bool blocking=false)
        {
            return get_element(blocking, false);
        }
        bool is_empty()
        {
            bool empty = false;

            mutex_.lock();
            empty = buffer_.empty();
            mutex_.unlock();

            return empty;
        }
    private:
        T get_element(bool blocking, bool remove)
        {
            T t;
            mutex_.lock();
            if (!buffer_.empty())
            {
                t = buffer_.front();
                if (remove)
                {
                  buffer_.pop_front();
                }
            }
            else
            {   
                if (blocking)
                {
                    while (true)
                    {
                        mutex_.unlock();
                        //wait for push
                        std::unique_lock<std::mutex> lck(notification_mutex_);
                        condition_var_.wait_for(lck, std::chrono::milliseconds(1000));
                        mutex_.lock();
                        if (!buffer_.empty())
                        {
                            t = buffer_.front();
                            if (remove)
                            {
                              buffer_.pop_front();
                            }
                            break;
                        }
                    }
                }
            }
            mutex_.unlock();
            return t;
        }
    private:
        bool empty_;
        std::deque<T> buffer_;
        std::condition_variable condition_var_;
        std::mutex mutex_;
        std::mutex notification_mutex_;
        unsigned int max_elements_;
        std::function<void (T& t)> drop_cb_;
    };
}

#endif
