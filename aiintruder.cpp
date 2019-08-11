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
    
    //setup FrameAnalyzer (NN) 
    FrameAnalyzer *analyzer = new FrameAnalyzer(options_->aiintruder.yolo_tiny, 0);
    analyzers_.push_back(analyzer);
   
    sessions_.reserve(options_->aiintruder.decoder_instances); //Session is not MoveInsertable 
    for (unsigned int i=0; i < options_->aiintruder.decoder_instances; i++)
    {
      JDecoder *j = new JDecoder();
      sessions_.emplace_back(nullptr, j);
    }

    //start analyzer threads
    for (auto a : analyzers_)
    {
      a->start();
    }

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
    static const char* alarm_string = "MDAlarm";
    if (strstr(path, alarm_string) != NULL)
    {
      int l = strlen(path);
#if 0
      if (memcmp(path+(l-4), ".mkv", 4) == 0)
      {
        return ALARM_TYPE_MKV;
      }
#endif
      if (memcmp(path+(l-4), ".jpg", 4) == 0)
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
    int found = 0;
    session_mutex.lock();
    for (unsigned int i=0; i<sessions_.size(); i++)
    {
      if (sessions_[i].free)
      {
        if ((type == ALARM_TYPE_JPG && sessions_[i].jdecoder) || 
            (type == ALARM_TYPE_MKV && sessions_[i].vdecoder))
        {
          sessions_[i].type = type;
          sessions_[i].free = false;
          found = i+1;
          break;
        }
      }
    }
    session_mutex.unlock();

    return found;
  }
  
  void AIIntruder::close_analyzer_pipe(int h)
  {
    h--;
    if (h < 0)
    {
      return;
    }
    Session& s = sessions_[h];
    if (s.type == ALARM_TYPE_JPG)
    {
      //send to frame analyzer!!!!
      cv::Mat mat = s.jdecoder->get_mat();
      for (auto a : analyzers_)
      {
        if (a->push_frame(mat))
        {
          break;
        }
        else
        {
          LOG_DBG("could not push");
        }
      }

    }
    else if (s.type == ALARM_TYPE_MKV)
    {

    }

    //release the decoder, free it again
    session_mutex.lock();
    sessions_[h].type = ALARM_TYPE_NOT;
    sessions_[h].free = true;
    session_mutex.unlock();
  }
  
  void AIIntruder::write(int h, const char* buf, int len)
  {
    h--;
    if (h < 0)
    {
      return;
    }
    Session& s = sessions_[h];
    if (s.type == ALARM_TYPE_JPG)
    {
      s.jdecoder->decode(buf, len);
    }
    else if (s.type == ALARM_TYPE_MKV)
    {

    }

  }
 

} //ns
