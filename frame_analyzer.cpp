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
#include <fstream>

#include "frame_analyzer.hpp"

namespace keymolen
{

    FrameAnalyzer::FrameAnalyzer(AsyncPipe<cv::Mat>& pipe_in, AsyncPipe<cv::Mat>& pipe_out) :
        frame_pipe_(pipe_in), result_pipe_(pipe_out), run_(false)
    {

    }

    FrameAnalyzer::~FrameAnalyzer()
    {

    }

    void FrameAnalyzer::start()
    {
        run_ = true;
        load_nn();
        thread_ = std::thread(&FrameAnalyzer::thread_loop, this);
    }

    void FrameAnalyzer::stop()
    {

    }

    
    void FrameAnalyzer::load_nn()
    {
        // Load names of classes
        std::string classesFile = "coco.names";
        std::ifstream ifs(classesFile.c_str());
        std::string line;
        while (std::getline(ifs, line))
        {
            classes_.push_back(line);
        }
        
        // Give the configuration and weight files for the model
        cv::String modelConfiguration = "yolov3.cfg";
        cv::String modelWeights = "yolov3.weights";

        // Load the network
        net_ = cv::dnn::readNetFromDarknet(modelConfiguration, modelWeights);
        net_.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
     
    }

    void FrameAnalyzer::thread_loop()
    {
        while(run_)
        {
            cv::Mat frame = frame_pipe_.pull(true); 
            cv::Mat result_frame = frame.clone();

            std::cout << "process frame" << std::endl;

            yolo(frame, result_frame);

            result_pipe_.push(result_frame);
        }
    }


    void FrameAnalyzer::yolo(cv::Mat& frame, cv::Mat& result)
    {
        cv::Mat blob;

        int width = frame.cols;
        int height = frame.rows;

        // Create a 4D blob from a frame.
        cv::dnn::blobFromImage(frame, blob, 1/255.0, cvSize(width, height), cv::Scalar(0,0,0), true, false);
        
        //Sets the input to the network
        net_.setInput(blob);
        
        // Runs the forward pass to get output of the output layers
        std::vector<cv::Mat> outs;
//        net_.forward(outs, getOutputsNames(net_));

    }


#if 0
    // Get the names of the output layers
    std::vector<cv::String> getOutputsNames(const Net& net)
    {
        static vector<String> names;
        if (names.empty())
        {
            //Get the indices of the output layers, i.e. the layers with unconnected outputs
            vector<int> outLayers = net.getUnconnectedOutLayers();

            //get the names of all the layers in the network
            vector<String> layersNames = net.getLayerNames();

            // Get the names of the output layers in names
            names.resize(outLayers.size());
            for (size_t i = 0; i < outLayers.size(); ++i)
                names[i] = layersNames[outLayers[i] - 1];
        }
        return names;
    }
#endif

}
