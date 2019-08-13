/*
 * Copyright (C) 2019, Bruno Keymolen (bruno.keymolen@gmail.com)
 */

#ifndef COMMON_ASYNC_BUFFER_HPP
#define COMMON_ASYNC_BUFFER_HPP

#include <condition_variable>
#include <mutex>

#include <string.h>
#include <stdlib.h>

namespace keymolen
{
  class AsyncBuffer
  {
  public:
  
    AsyncBuffer(int size) : buf_len_(0), data_start_(0), data_len_(0), wait_for_data_(0)
    {
      buf_ = (char*)malloc(size);
    }

    ~AsyncBuffer()
    {
      free(buf_);
    }

    // [dataA|          ]
    // [dataA|dataB |   ]
    // [      |dataB|   ]  
    // [aC|   |dataB|dat] (rollover)
    int push(const char* data, int len)
    {
      mutex_.lock();
      if (buf_len_ == data_len_)
      {
        mutex_.unlock();
        return 0;
      }

      int empty = buf_len_ - data_len_;
      len = std::min(len, empty); //copy what we can
   
      int first_part_len = std::min(len, buf_len_-data_start_);
      int second_part_len = std::max(0, len-first_part_len);

      memcpy(buf_+data_start_, data, first_part_len);
      if (second_part_len)
      {
        memcpy(buf_, data+first_part_len, second_part_len);
      }
      data_len_ += len;

      if (wait_for_data_ > 0 && data_len_ >= wait_for_data_)
      {
          condition_var_.notify_one();
      }

      mutex_.unlock();
      
      return len;
    }
    
    int pull(char* data, int len, bool exact_len=false)
    {
      mutex_.lock();
      if (data_len_ < len)
      {
        wait_for_data_ = len;
        std::unique_lock<std::mutex> lck(notification_mutex_);
        mutex_.unlock();
        //possible the data is received right here, in that case we wrongly wait
        condition_var_.wait_for(lck, std::chrono::milliseconds(wait_pull_ms_));
        mutex_.lock();
      }
      wait_for_data_ = 0;

      if (exact_len && data_len_ < len)
      {
        mutex_.unlock();
        return 0;
      }

      len = std::min(len, data_len_);
      int first_part_len = std::max(len, buf_len_-data_start_);
      int second_part_len = std::max(0, len-first_part_len);

      memcpy(data, buf_+data_start_, first_part_len);
      if (second_part_len)
      {
        memcpy(data+first_part_len, buf_, second_part_len);
      }
      data_start_ = (data_start_+len) % buf_len_;
      data_len_ -= len;

      mutex_.unlock();
      return len;
    }
 
  private:
    char* buf_;
    const int buf_len_;

    int data_start_;
    int data_len_;

    std::mutex mutex_;
    std::condition_variable condition_var_;
    std::mutex notification_mutex_;
    
    int wait_pull_ms_;
    int wait_for_data_;
  };

} //ns

#endif
