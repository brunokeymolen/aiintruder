// **********************************************************************************
//
// BSD License.
//
// Copyright (c) 2019, Bruno Keymolen, email: bruno.keymolen@gmail.com
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright notice, this
// list of conditions and the following disclaimer in the documentation and/or other
// materials provided with the distribution.
// Neither the name of "Bruno Keymolen" nor the names of its contributors may be
// used to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// **********************************************************************************

#include <stdlib.h>
#include <string.h>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"

#include "jdecoder.hpp"

namespace keymolen
{

  JDecoder::JDecoder() : data_len_(0)
  {
    buflen_ = 30000;
    buf_ = (char*)malloc(buflen_);
  }

  JDecoder::~JDecoder()
  {
    free(buf_);
  }

  int JDecoder::decode(char* data, int len)
  {
    if (len > 0)
    {
      if (len + data_len_ > buflen_)
      {
        buflen_ += 10000;
        buf_ = (char*)realloc(buf_, buflen_);
      }
      memcpy(buf_+data_len_, data, len);
      data_len_+=len;
    }
    return data_len_;
  }

  cv::Mat JDecoder::get_mat()
  {
    cv::Mat m = cv::imdecode(cv::Mat(1, data_len_, CV_8UC1, buf_), cv::IMREAD_UNCHANGED);
    data_len_ = 0;
    return m;
  }

 
} //ns

