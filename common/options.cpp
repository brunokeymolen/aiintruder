#include "options.hpp"

#include <unistd.h>

namespace keymolen {

    Options *Options::instance__ = NULL;

    Options::Options()
    {
    }

    Options::~Options()
    {
    }

    Options::FTPServer::FTPServer() :
        root("/tmp/"),
        port(2121),
        save(false)
    { 
    }
    
    Options::AIIntruder::AIIntruder() :
         gui(false),
         yolo_tiny(true),
         process_interval_sec(3),
         backlog_size_ram(6),
         decoder_instances(2),
         analyzer_instances(1)
    {
    }

    Options::Log::Log() :
        path("/tmp/aiintruder.log")
    {
    }


    Options *Options::Instance()
    {
        if (instance__ == NULL)
        {
            instance__ = new Options();
        }
        return instance__;
    }

    void Options::cleanup()
    {
        if (instance__ != NULL)
        {
            delete (instance__);
            instance__ = NULL;
        }
    }

    int Options::Read(const char* path)
    {
        try
        {
            cfg.readFile(path);
        }
        catch(const libconfig::FileIOException &fioex)
        {
            std::cout << "Could not load config file, I/O error: " << path << std::endl;
            return -1;
        }
        catch(const libconfig::ParseException &pex)
        {
            std::cout << "Parse error at " << pex.getFile() << ":" << pex.getLine()  << " - " << pex.getError() << std::endl;
            return -1;
        }
        catch(...)
        {
            std::cout << "Could not load config file, : " << path << std::endl;
            return -1;
        }

        libconfig::Setting &root = cfg.getRoot();

        if(cfg.exists("ftpserver"))
        {
            libconfig::Setting &entry = root["ftpserver"];
            entry.lookupValue("root", *(const_cast<std::string*>(&(ftpserver.root))));
            entry.lookupValue("port", *(const_cast<unsigned int*>(&(ftpserver.port))));
            entry.lookupValue("save", *(const_cast<bool*>(&(ftpserver.save))));
        }

        if(cfg.exists("aiintruder"))
        {
            libconfig::Setting &entry = root["aiintruder"];
            entry.lookupValue("gui", *(const_cast<bool*>(&(aiintruder.gui))));
            entry.lookupValue("yolo_tiny", *(const_cast<bool*>(&(aiintruder.yolo_tiny))));
            entry.lookupValue("process_interval_sec", *(const_cast<unsigned int*>(&(aiintruder.process_interval_sec))));
            entry.lookupValue("backlog_size_ram", *(const_cast<unsigned int*>(&(aiintruder.backlog_size_ram))));
            entry.lookupValue("decoder_instances", *(const_cast<unsigned int*>(&(aiintruder.decoder_instances))));
            entry.lookupValue("analyzer_instances", *(const_cast<unsigned int*>(&(aiintruder.analyzer_instances))));
            entry.lookupValue("intruder_path", *(const_cast<std::string*>(&(aiintruder.intruder_path))));
       }

       return 0;
    }

} //ns


