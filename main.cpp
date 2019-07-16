/*
 * Copyright 2019 Bruno Keymolen (bruno.keymolen@gmail.com)
 *
 * This is unpublished proprietary source code of Bruno Keymolen (bruno.keymolen@gmail.com).
 * The copyright notice above does not evidence any actual or
 * intended publication of such source code.
 *
 * Redistribution of this material is strictly prohibited.
 */
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <map>
#include <iostream>
#include <thread>
#include <vector>


#include <opencv2/highgui/highgui_c.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include "common/async_pipe.hpp"
#include "frame_analyzer.hpp"

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



#define PIPE_CNT 5
struct s_pipe_info
{
    const char* path;
    int fd;
};
struct s_pipe_info pipe_info[PIPE_CNT] = { 
                                { "/tmp/cam-00.pipe", 0},
                                { "/tmp/cam-01.pipe", 0},
                                { "/tmp/cam-02.pipe", 0},
                                { "/tmp/cam-03.pipe", 0},
                                { "/tmp/cam-04.pipe", 0}
                              };


const char* CW_IMG_ORIGINAL 	= "Original";
const char* CW_DETECTION  	  = "Detection";


void analyze(std::string video_path, keymolen::AsyncPipe<cv::Mat>* result_pipe, int id);


void usage(char * s)
{

	fprintf( stderr, "\n");
    	fprintf( stderr, "%s -v <video file>  -w <pause between frames in ms> [-x](nogui) [-t](tiny) \nbuild: %s-%s \n", s, __DATE__, __TIME__);
	fprintf( stderr, "\n");
}

int main(int argc, char** argv) {

    int c;
    while ( ((c = getopt( argc, argv, "txw:v:?" ) ) ) != -1 )
    {
        switch (c)
        {
            case 't':
                options.tiny = true;
                break;
             case 'x':
                options.nogui = true;
                break;
            case 'v':
               options.video_path = optarg;
                break;
            case 'w':
                options.w = atoi(optarg);
            break;
                case '?':
            default:
                usage(argv[0]);
                return -1;
        }
    }

    if(options.video_path.empty())
    {
        usage(argv[0]);
        return -1;
    }

    if (!options.nogui)
    {
        cv::namedWindow(CW_IMG_ORIGINAL, cv::WINDOW_AUTOSIZE);
        cv::namedWindow(CW_DETECTION, 	 cv::WINDOW_AUTOSIZE);

        cv::moveWindow(CW_IMG_ORIGINAL, 10, 10);
        cv::moveWindow(CW_DETECTION, 680, 10);
    }

    keymolen::AsyncPipe<cv::Mat> result_pipe;

    //start the analyzer threads
    std::vector<std::thread> threads;
    for (int i=0; i<PIPE_CNT; i++)
    {
      threads.emplace_back(&analyze, pipe_info[i].path, &result_pipe, i);
    }

    //result thread
    cv::Mat analyzed_frame;
    while(true)
    {
        analyzed_frame = result_pipe.pull(true);
        std::cout << "result..." << std::endl;

        if (!options.nogui)
        {
          imshow(CW_DETECTION, analyzed_frame);
        }

    }

    return 0;
}



void analyze(std::string video_path, keymolen::AsyncPipe<cv::Mat>* r, int id)
{
    keymolen::AsyncPipe<cv::Mat>& result_pipe = *r;

    cv::Mat frame;
    keymolen::AsyncPipe<cv::Mat> detector_pipe;
    keymolen::FrameAnalyzer frame_analyzer(detector_pipe, result_pipe, options.tiny, id);
    frame_analyzer.start();

    uint64_t pushed = 0;
    uint64_t dropped = 0;


    while (true)
    {

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
            if (!options.nogui)
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

            if (options.w)
            {
                if (cv::waitKey(options.w) == 27)
                {
                    break;
                }
            }
        }
        
    }
       
    std::cout << "ending..." << std::endl;
    frame_analyzer.stop();

}

