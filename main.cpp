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

#include <opencv2/highgui/highgui_c.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include "common/async_pipe.hpp"
#include "frame_analyzer.hpp"

//extern FILE *stdin;
//extern FILE *stdout;
//extern FILE *stderr;

std::string video_path;
int w = 0;
bool  nogui = false;

const char* CW_IMG_ORIGINAL 	= "Original";
const char* CW_DETECTION  	  = "Detection";


void analyze(std::string video_path);


void usage(char * s)
{

	fprintf( stderr, "\n");
    	fprintf( stderr, "%s -v <video file>  -w <pause between frames in ms> [-x](nogui)\nbuild: %s-%s \n", s, __DATE__, __TIME__);
	fprintf( stderr, "\n");
}

int main(int argc, char** argv) {

    int c;
    while ( ((c = getopt( argc, argv, "xw:v:?" ) ) ) != -1 )
    {
        switch (c)
        {
            case 'x':
                nogui = true;
                break;
            case 'v':
               video_path = optarg;
                break;
            case 'w':
                w = atoi(optarg);
            break;
                case '?':
            default:
                usage(argv[0]);
                return -1;
        }
    }

    if(video_path.empty())
    {
        usage(argv[0]);
        return -1;
    }

    if (!nogui)
    {
        cv::namedWindow(CW_IMG_ORIGINAL, cv::WINDOW_AUTOSIZE);
        cv::namedWindow(CW_DETECTION, 	 cv::WINDOW_AUTOSIZE);

        cv::moveWindow(CW_IMG_ORIGINAL, 10, 10);
        cv::moveWindow(CW_DETECTION, 680, 10);
    }

    analyze(video_path);

    return 0;
}



void analyze(std::string video_path)
{
    cv::Mat frame;
    cv::Mat analyzed_frame;
    keymolen::AsyncPipe<cv::Mat> detector_pipe;
    keymolen::AsyncPipe<cv::Mat> result_pipe;
    keymolen::FrameAnalyzer frame_analyzer(detector_pipe, result_pipe);
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
            if (!nogui)
            {
                imshow(CW_IMG_ORIGINAL, frame);
            }

            if (!result_pipe.is_empty())
            {
                std::cout << "result..." << std::endl;

                analyzed_frame = result_pipe.pull();
                
                //cv::imwrite("some.jpg", cvmat);

                if (!nogui)
                {
                    imshow(CW_DETECTION, analyzed_frame);
                }

            }

            if(detector_pipe.push(frame))
            {
                pushed++;
            }
            else
            {
                dropped++;
            }

            if (w)
            {
                if (cv::waitKey(w) == 27)
                {
                    break;
                }
            }
        }
        
    }
       
    std::cout << "ending..." << std::endl;
    frame_analyzer.stop();

}

