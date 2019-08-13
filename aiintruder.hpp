/*
 * Copyright 2019 Bruno Keymolen (bruno.keymolen@gmail.com)
 *
 * This is unpublished proprietary source code of Bruno Keymolen (bruno.keymolen@gmail.com).
 * The copyright notice above does not evidence any actual or
 * intended publication of such source code.
 *
 * Redistribution of this material is strictly prohibited.
 */
#ifndef KEYMOLEN_AIINTRUDER_HPP
#define KEYMOLEN_AIINTRUDER_HPP

#include <thread>
#include <vector>

#include "common.hpp"
#include "async_pipe.hpp"
#include "async_buffer.hpp"
#include "ftphook.hpp"
#include "options.hpp"
#include "frame_analyzer.hpp"
#include "vdecoder.hpp"
#include "jdecoder.hpp"

#include <opencv2/highgui/highgui_c.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#define ALARM_TYPE_NOT 0
#define ALARM_TYPE_MKV 1
#define ALARM_TYPE_JPG 2
#define ALARM_TYPE_UNKNOWN 3

namespace keymolen
{
    
    class AIIntruder : public FTPHook
    {
    private:
      struct Session
      {
        Session(VDecoder* v, JDecoder* j) 
          : free(true), type(ALARM_TYPE_NOT), vdecoder(v), jdecoder(j)
        {
        }
        ~Session()
        {
          if (vdecoder)
          {
            delete(vdecoder);
          }
          if (jdecoder)
          {
            delete (jdecoder);
          }
        }
        bool free;
        int type;
        VDecoder *vdecoder;
        JDecoder *jdecoder;
        
        Session& operator=(const Session& other) = default;
//        AsyncBuffer data;
      };
      std::mutex session_mutex;
      Session *session_get_free();
      void session_put(Session* session);
      void write(char* data, int len); //0 = eof
    public:
        AIIntruder();
        virtual ~AIIntruder();
    public:
        void start();
        void stop();
    public:
        //FTPHook
        virtual int is_alarm(const char* path);
        virtual int open_analyzer_pipe(int type);
        virtual void close_analyzer_pipe(int h);
        virtual void write(int h, const char* buf, int len);
 
    private:
        Options *options_;
        bool run_;
        std::vector<FrameAnalyzer*> analyzers_;
        std::vector<Session> sessions_;
        //std::vector<VDecoder*> decoders_;
    };

}

#endif
