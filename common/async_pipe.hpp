/*
 * Copyright (C) 2019, Bruno Keymolen (bruno.keymolen@gmail.com)
 */

#ifndef COMMON_ASYNC_PIPE_HPP
#define COMMON_ASYNC_PIPE_HPP

#include <condition_variable>
#include <mutex>


namespace keymolen {

    template <typename T> class AsyncPipe 
    {
    public:
        AsyncPipe() : empty_(true)
        {

        }
        bool push(T& t)
        {
            bool r = false;
            mutex_.lock();
            if (empty_)
            {
                buffer_ = t;
                empty_ = false;
                r = true;
            }
            mutex_.unlock();
            condition_var_.notify_one();
            return r;
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
            return empty_;
        }
    private:
        T get_element(bool blocking, bool remove)
        {
            T t;
            mutex_.lock();
            if (!empty_)
            {
                t = buffer_;
                if (remove)
                {
                    empty_ = true;
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
                        if (!empty_)
                        {
                            t = buffer_;
                            if (remove)
                            {
                                empty_ = true;
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
        T buffer_;
        std::condition_variable condition_var_;
        std::mutex mutex_;
        std::mutex notification_mutex_;

    };
}

#endif
