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

    Options::Pipes::Pipes() 
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
         process_interval_sec(3)
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

        if(cfg.exists("pipes"))
        {
            libconfig::Setting &entry = root["pipes"];
            libconfig::Setting& pipe_paths = entry["pipes_path"];
            int count = pipe_paths.getLength();
            std::cout << "pipes cnt: " << count << std::endl;

            for (int i=0; i<count; i++)
            {
                std::string p = pipe_paths[i];
                pipes.pipe_paths.push_back(p);
                std::cout << p << std::endl;
            }
        }

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
       }

       return 0;
    }

} //ns


