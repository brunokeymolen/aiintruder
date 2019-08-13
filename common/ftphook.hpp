#ifndef KEYMOLEN_FTPHOOK_H
#define KEYMOLEN_FTPHOOK_H

namespace keymolen
{

  class FTPHook
  {
  public:
    virtual int is_alarm(const char* path) = 0;
    virtual int open_analyzer_pipe(int type) = 0;
    virtual void close_analyzer_pipe(int h) = 0;
    virtual void write(int h, const char* buf, int len) = 0;
  };

} //ns

#endif
