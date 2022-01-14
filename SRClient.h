#pragma once
#include "ffmpegConfig.h"
#include "opencvConfig.h"
//#include <list>

class SRClient
{
public:
	SRClient();
	~SRClient();

private:
	void cvMatToAVFrame(cv::Mat& mat, AVFrame*& frame);

};

