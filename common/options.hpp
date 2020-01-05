/*
 * Author: Bruno Keymolen 
 *
 */
#ifndef AIINTRUDER_OPTIONS_HPP
#define AIINTRUDER_OPTIONS_HPP

#include "common.hpp"

#include <libconfig.h++>
#include <vector>

namespace keymolen
{
    class Options
    {
            static Options *instance__;
        private:
            Options();
            virtual ~Options();
        public:
            static Options *Instance();
            int Read(const char* path);
            void cleanup();
        public:
            struct FTPServer
            {
                FTPServer();
                const std::string root;
                const unsigned int port;
                const bool save;
            } ftpserver;

            struct AIIntruder
            {
                AIIntruder();
                const bool gui;
                const bool yolo_tiny; 
                const unsigned int process_interval_sec;
                const unsigned int backlog_size_ram;

                const unsigned int decoder_instances;
                const unsigned int analyzer_instances;
                const std::string intruder_path;
                const std::string dropped_frames_path;
                const bool process_dropped_frames;
            } aiintruder;
           
            struct Log
            {
                Log();
                const std::string path;
            } log;

        private:
            libconfig::Config cfg;
    };

}

#endif
