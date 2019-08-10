/*
 * Copyright 2019 Bruno Keymolen (bruno.keymolen@gmail.com)
 *
 * This is unpublished proprietary source code of Bruno Keymolen (bruno.keymolen@gmail.com).
 * The copyright notice above does not evidence any actual or
 * intended publication of such source code.
 *
 * Redistribution of this material is strictly prohibited.
 */
#include <stdlib.h>

#include "aiintruder.hpp"

namespace keymolen
{
  

  AIIntruder::AIIntruder() 
  {
    options_ = Options::Instance();
  }
        
  AIIntruder::~AIIntruder()
  {

  }
    
  void AIIntruder::AIIntruder::start()
  {
    
    // flow:
    // ftpserver -> aiintruder -> decoder -> aiintruder -> frame-analyzer -> notification
   

    //create notification class
    
    //setup FrameAnalyzer (NN) instances and pass notification 
    for (unsigned int i=0; i < options_->aiintruder.analyzer_instances; i++)
    {
      FrameAnalyzer *analyzer = new FrameAnalyzer(options_->aiintruder.yolo_tiny, i);
      analyzers_.push_back(analyzer);
    }
    
    //setup decoders (assign a frame analyzer to it, multiple decoders can share one frame analyzer) 
    for (unsigned int i=0; i < options_->aiintruder.decoder_instances; i++)
    {
      VDecoder *decoder = new VDecoder(i);
      sessions_.emplace_back(decoder); 
    }

    //start analyzer threads
    for (auto a : analyzers_)
    {
      a->start();
    }

    //start decoder threads
    

  }
  
  void AIIntruder::AIIntruder::stop()
  {
    run_ = true;
    //stop decoders
    //stop frame analyzers
    for (auto a : analyzers_)
    {
      delete(a);
    }
  }
    
  //
  // FTPHook
  // 
  int AIIntruder::is_alarm(const char* path)
  {
    static const char* alarm_string = "MDalarm";
    if (strstr(path, alarm_string) != NULL)
    {
      int l = strlen(path);
      if (strstr(path+(l-4), ".mkv") == 0)
      {
        return ALARM_TYPE_MKV;
      }
      if (strstr(path+(l-4), ".jpg") == 0)
      {
        return ALARM_TYPE_JPG;
      }
      return ALARM_TYPE_UNKNOWN;
    }
      
    return ALARM_TYPE_NOT;
  }
  
  int AIIntruder::open_analyzer_pipe(int type)
  {
    //get a free decoder and open it
    int found = -1;
    session_mutex.lock();
    for (unsigned int i=0; i<sessions_.size(); i++)
    {
      if (sessions_[i].free)
      {
        sessions_[i].free = false;
        found = i;
        break;
      }
    }
    session_mutex.unlock();

    return found;
  }
  
  void AIIntruder::close_analyzer_pipe(int h)
  {
    //release the decoder, free it again
    session_mutex.lock();
    sessions_[h].free = true;
    session_mutex.unlock();
  }
  
  void AIIntruder::write(int h, const char* buf, int len)
  {
    //keep data to pass data to the decoder at a later time
    //sessions_[h].data.push(buf, len);
  }
 

} //ns
