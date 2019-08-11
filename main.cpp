/*
 * Copyright 2019 Bruno Keymolen (bruno.keymolen@gmail.com)
 *
 * This is unpublished proprietary source code of Bruno Keymolen (bruno.keymolen@gmail.com).
 * The copyright notice above does not evidence any actual or
 * intended publication of such source code.
 *
 * Redistribution of this material is strictly prohibited.
 */
#include "common.hpp"

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <map>
#include <iostream>
#include <thread>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <opencv2/highgui/highgui_c.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include "common/async_pipe.hpp"
#include "frame_analyzer.hpp"
#include "common/options.hpp"

#include "aiintruder.hpp"
#include "ftpserver/ftpserver.h"

#if 0
//extern FILE *stdin;
//extern FILE *stdout;
//extern FILE *stderr;




struct Options
{
  std::string video_path;
  int w = 0;
  bool  nogui = false;
  bool tiny = false;
} options;


std::string config_path;

#define PIPE_CNT 1
struct s_pipe_info
{
    const char* path;
    int fd;
};
struct s_pipe_info pipe_info[5] = { 
                                { "/tmp/cam-00.pipe", 0},
                                { "/tmp/cam-01.pipe", 0},
                                { "/tmp/cam-02.pipe", 0},
                                { "/tmp/cam-03.pipe", 0},
                                { "/tmp/cam-04.pipe", 0}
                              };

const char* CW_IMG_ORIGINAL 	= "Original";
const char* CW_DETECTION  	  = "Detection";
#endif


//void analyze(std::string video_path, keymolen::AsyncPipe<cv::Mat>* result_pipe, int id);


void usage(char * s)
{

	fprintf( stderr, "\n");
 	fprintf( stderr, "%s : %s-%s \n", s, __DATE__, __TIME__);
	fprintf( stderr, "%s -c <config file>\n", s);
	fprintf( stderr, "example: %s -c conf/aiintruder.conf\n", s);
	fprintf( stderr, "\n");
}

int main(int argc, char** argv) {
    std::string config_path;

    int c;
    while ( ((c = getopt( argc, argv, "c:?" ) ) ) != -1 )
    {
        switch (c)
        {
            case 'c':
                config_path = optarg;
                break;
            case '?':
            default:
                usage(argv[0]);
                return -1;
        }
    }

    if(config_path.empty())
    {
        usage(argv[0]);
        return -1;
    }

    keymolen::Options *options = keymolen::Options::Instance();
    if (options->Read(config_path.c_str()) != 0)
    {
        std::cout << "invalid config file! " << config_path << std::endl;
        return -1;
    }

    keymolen::AIIntruder aiintruder;

    struct FtpServer *s = (struct FtpServer*) malloc(sizeof(struct FtpServer));
    s->_port = options->ftpserver.port;
    strncpy(s->_relative_path, options->ftpserver.root.c_str(), 100);
    s->_options = options;
    s->_hook = dynamic_cast<keymolen::FTPHook*>(&aiintruder);
 
    aiintruder.start(); 

    init_ftp_server(s);
    start_ftp_server(s);

    LOG_DBG("end...");
    aiintruder.stop();


    return 0;
}


#if 0
    if (options->aiintruder.gui)
    {
        cv::namedWindow(CW_IMG_ORIGINAL, cv::WINDOW_AUTOSIZE);
        cv::namedWindow(CW_DETECTION, 	 cv::WINDOW_AUTOSIZE);

        cv::moveWindow(CW_IMG_ORIGINAL, 10, 10);
        cv::moveWindow(CW_DETECTION, 680, 10);
    }

    keymolen::AsyncPipe<cv::Mat> result_pipe;

    //start the analyzer threads
    std::vector<std::thread> threads;
    for (unsigned int i=0; i<options->pipes.pipe_paths.size(); i++)
    {
      threads.emplace_back(&analyze, options->pipes.pipe_paths[i], &result_pipe, i);
    }

    //result thread
    cv::Mat analyzed_frame;
    while(true)
    {
        analyzed_frame = result_pipe.pull(true);
        std::cout << "result..." << std::endl;

        if (options->aiintruder.gui)
        {
          imshow(CW_DETECTION, analyzed_frame);
        }

    }

    return 0;
}



void analyze(std::string video_path, keymolen::AsyncPipe<cv::Mat>* r, int id)
{
    std::cout << "setup thread id: " << keymolen::gettid() << " path: " << video_path << std::endl;

    keymolen::Options *options = keymolen::Options::Instance();
    
    keymolen::AsyncPipe<cv::Mat>& result_pipe = *r;

    cv::Mat frame;
    keymolen::AsyncPipe<cv::Mat> detector_pipe;
    keymolen::FrameAnalyzer frame_analyzer(detector_pipe, result_pipe, options->aiintruder.yolo_tiny, id);
    frame_analyzer.start();

    uint64_t pushed = 0;
    uint64_t dropped = 0;


    while (true)
    {

        std::cout << "opening : " << video_path << std::endl; 

        cv::VideoCapture reader(video_path);

        if (!reader.isOpened())
        {
            std::cout << "could not open: " << video_path << std::endl;
            sleep(1);
            continue;
        }

        std::cout << "opened : " << video_path << std::endl; 

        while (true)
        {
            reader >> frame;
            if (frame.empty())
            {
                std::cout << "eof..." << std::endl;
                break;
            }
            if (options->aiintruder.gui)
            {
                imshow(CW_IMG_ORIGINAL, frame);
            }

            if(detector_pipe.push(frame))
            {
                pushed++;
            }
            else
            {
                dropped++;
            }

            if (options->aiintruder.gui)
            {
                if (cv::waitKey(20) == 27)
                {
                    break;
                }
            }
        }
        
    }
       
    std::cout << "ending..." << std::endl;
    frame_analyzer.stop();

}
#endif



