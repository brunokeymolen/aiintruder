/*
 * Copyright 2019 Bruno Keymolen (bruno.keymolen@gmail.com)
 *
 * This is unpublished proprietary source code of Bruno Keymolen (bruno.keymolen@gmail.com).
 * The copyright notice above does not evidence any actual or
 * intended publication of such source code.
 *
 * Redistribution of this material is strictly prohibited.
 */
#ifndef KEYMOLEN_FRAME_ANALYZER_HPP
#define KEYMOLEN_FRAME_ANALYZER_HPP

#include <thread>
#include "common.hpp"
#include "common/async_pipe.hpp"

#include <opencv2/dnn.hpp>
#include <opencv2/highgui/highgui_c.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

namespace keymolen
{

    class FrameAnalyzer
    {
    public:
        FrameAnalyzer(AsyncPipe<cv::Mat>& pipe_in, AsyncPipe<cv::Mat>& pipe_out);
        virtual ~FrameAnalyzer();
    public:
        void start();
        void stop();
    private:
        AsyncPipe<cv::Mat>& frame_pipe_;
        AsyncPipe<cv::Mat>& result_pipe_;
        std::thread thread_;
        bool run_;
        std::vector<std::string> classes_;
        cv::dnn::Net net_;
    private:
        void thread_loop();
        void yolo(cv::Mat& frame, cv::Mat& result);
        void load_nn();
     };

}

#endif
