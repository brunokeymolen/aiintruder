
#include <stdlib.h>

namespace keymolen {
    inline uint64_t getms()                                                                                                                                                                                                                                                       
    {                                                                                                                                         
      uint64_t ms;                                                                                                                          
      struct timespec spec;                                                                                                                 
      clock_gettime(CLOCK_REALTIME, &spec);                                                                                                 
      ms = spec.tv_sec * 1000;                                                                                                              
      ms += ((double) spec.tv_nsec / 1000000.0);                                                                                            
      return ms;                                                                                                                            
    }                      


}
