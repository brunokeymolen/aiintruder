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

#include "options.hpp"
#include "frame_analyzer.hpp"

namespace keymolen
{

    FrameAnalyzer::FrameAnalyzer(bool tiny, int id) :
        run_(false), tiny_(tiny), id_(id)
    {
        load_intruder_classes();
        intruder_img_path_ = Options::Instance()->aiintruder.intruder_path;

        if (tiny_)
        {
          model_configuration_ = "yolov3-tiny.cfg";
          model_weights_ = "yolov3-tiny.weights";
        }
        else
        {
          model_configuration_ = "yolov3.cfg";
          model_weights_ = "yolov3.weights";
        }
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

    bool FrameAnalyzer::push_frame(cv::Mat& frame)
    {
      return frame_pipe_.push(frame);
    }
    
    void FrameAnalyzer::load_intruder_classes()
    {
        std::string classesFile = "intruder.classes";
        std::ifstream ifs(classesFile.c_str());
        std::string line;
        while (std::getline(ifs, line))
        {
            if (line[0] != '#')
            {
                intruder_classes_.push_back(line);
            }
        }
 
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
        
        // Load the network
        net_ = cv::dnn::readNetFromDarknet(model_configuration_, model_weights_);
        
#if 0
        //VPU (like Intel Movidius Myriad X / Intel Neural Compute Stick 2)
        //https://www.learnopencv.com/using-openvino-with-opencv/
        net_.setPreferableBackend(DNN_BACKEND_INFERENCE_ENGINE);
#else
        net_.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
#endif

        net_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
     
        get_output_names();
    }

    void FrameAnalyzer::thread_loop()
    {
        bool hit = false;
        while(run_)
        {
            cv::Mat frame = frame_pipe_.peek(true).clone(); 
            cv::Mat result_frame;

            uint64_t start = getms();
            LOG_DBG("process frame");
            
            yolo(frame, result_frame, hit);

            cv::Mat boxresult = result_frame.clone();

            //TODO
            //result_pipe_.push(boxresult);

            if (hit)
            {
                std::ostringstream names;
                names << "alarm." << id_ << "-" << time(0) << ".jpg";

                std::ostringstream os;
                os << intruder_img_path_;
                os << names.str();
                std::string fname = os.str();
                cv::imwrite(fname.c_str(), boxresult);

                std::string img_name = names.str();

                //notification
                char pb[512];
                getcwd(pb, 512);

                //Move this to another thread since it stalls the frame analyzer for 2 seconds with my isp....
                std::ostringstream notify;
                notify << pb << "/notify.sh " << fname << " " << img_name;
                LOG_DBG(notify.str());
                system(notify.str().c_str());
            }

            uint64_t stop = getms();

            LOG_DBG("object detection time: " << stop-start << " ms.")

            //remove from the pipe, make place for new
            frame_pipe_.pull(false); 
        }
    }


    void FrameAnalyzer::yolo(cv::Mat& frame, cv::Mat& result, bool& hit)
    {
        cv::Mat blob;

        // Create a 4D blob from a frame.
        cv::dnn::blobFromImage(frame, blob, 1/255.0, cvSize(inpWidth, inpHeight), cv::Scalar(0,0,0), true, false);
        
        //Sets the input to the network
        net_.setInput(blob);
        
        std::cout << "run" << std::endl;
        // Runs the forward pass to get output of the output layers
        std::vector<cv::Mat> outs;
        net_.forward(outs, outlayer_names_);

        postprocess(frame, outs, hit);
        
        frame.convertTo(result, CV_8U);

        std::cout << "outs cnt: " << outs.size() << std::endl;

    }


    // Get the names of the output layers
    void FrameAnalyzer::get_output_names()
    {
        if (!outlayer_names_.empty())
        {
            return;
        }
    
        //Get the indices of the output layers, i.e. the layers with unconnected outputs
        std::vector<int> outLayers = net_.getUnconnectedOutLayers();

        //get the names of all the layers in the network
        std::vector<cv::String> layersNames = net_.getLayerNames();

        // Get the names of the output layers in names
        outlayer_names_.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
        {
            outlayer_names_[i] = layersNames[outLayers[i] - 1].c_str();
        }
    
    }

    // Remove the bounding boxes with low confidence using non-maxima suppression
    void FrameAnalyzer::postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs, bool& hit)
    {
        hit = false;

        std::vector<int> classIds;
        std::vector<float> confidences;
        std::vector<cv::Rect> boxes;

        for (size_t i = 0; i < outs.size(); ++i)
        {
            // Scan through all the bounding boxes output from the network and keep only the
            // ones with high confidence scores. Assign the box's class label as the class
            // with the highest score for the box.
            float* data = (float*)outs[i].data;
            for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
            {
                cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
                cv::Point classIdPoint;
                double confidence;
                // Get the value and location of the maximum score
                minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
                if (confidence > confThreshold)
                {
                    int centerX = (int)(data[0] * frame.cols);
                    int centerY = (int)(data[1] * frame.rows);
                    int width = (int)(data[2] * frame.cols);
                    int height = (int)(data[3] * frame.rows);
                    int left = centerX - width / 2;
                    int top = centerY - height / 2;

                    classIds.push_back(classIdPoint.x);
                    confidences.push_back((float)confidence);
                    boxes.push_back(cv::Rect(left, top, width, height));
                }
            }
        }

        // Perform non maximum suppression to eliminate redundant overlapping boxes with
        // lower confidences
        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
        for (size_t i = 0; i < indices.size(); ++i)
        {
            bool intruder = false;
            int idx = indices[i];
            for (auto in : intruder_classes_)
            {
                if (classes_[classIds[idx]] == in)
                {
                    intruder = true;
                    break;
                }
            }
            if (intruder)
            {
                cv::Rect box = boxes[idx];
                drawPred(classIds[idx], confidences[idx], box.x, box.y,
                    box.x + box.width, box.y + box.height, frame);
                hit = true;
            }
        }
    }

    // Draw the predicted bounding box
    void FrameAnalyzer::drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame)
    {
        //Draw a rectangle displaying the bounding box
        rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(255, 178, 50), 3);

        //Get the label for the class name and its confidence
        std::string label = cv::format("%.2f", conf);
        if (!classes_.empty())
        {
            if(classId < (int)classes_.size())
            {
                label = classes_[classId] + ":" + label;
                std::cout << time(0) << " : " << label << std::endl;
            }
            else
            {
                label = "unknown";
            }
        }

        //Display the label at the top of the bounding box
        int baseLine;
        cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
        top = std::max(top, labelSize.height);
        rectangle(frame, cv::Point(left, top - std::round(1.5*labelSize.height)), cv::Point(left + round(1.5*labelSize.width), top + baseLine), cv::Scalar(255, 255, 255), cv::FILLED);
        putText(frame, label, cv::Point(left, top), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0,0,0),1);
    }



} //ns
