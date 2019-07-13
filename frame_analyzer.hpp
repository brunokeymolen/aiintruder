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
#include <vector>

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
        const float confThreshold = 0.5; // Confidence threshold
        const float nmsThreshold = 0.4;  // Non-maximum suppression threshold
        const int inpWidth = 416;  // Width of network's input image
        const int inpHeight = 416; // Height of network's input image
    private:
        AsyncPipe<cv::Mat>& frame_pipe_;
        AsyncPipe<cv::Mat>& result_pipe_;
        std::thread thread_;
        bool run_;
        std::vector<std::string> classes_;
        cv::dnn::Net net_;
        std::vector<cv::String> outlayer_names_;
    private:
        void thread_loop();
        void yolo(cv::Mat& frame, cv::Mat& result);
        void load_nn();
        void get_output_names();
        void postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs);
        void drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame);
     };

}

#endif
