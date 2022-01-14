#include "SRClient.h"

SRClient::SRClient()
{
}

SRClient::~SRClient()
{
}

void SRClient::cvMatToAVFrame(cv::Mat& mat, AVFrame*& frame)
{
	if (!frame) {
		frame = av_frame_alloc();
		frame->format = AV_PIX_FMT_YUV420P;
		frame->width = mat.cols;
		frame->height = mat.rows;
	}
	if (frame && !mat.empty()) {
		av_frame_get_buffer(frame, 0);
		av_frame_make_writable(frame);
		
		cv::Mat yuv;
		cv::cvtColor(mat, yuv, cv::COLOR_BGR2YUV_I420);
		int frame_size = mat.cols * mat.rows;
		unsigned char* pdata = yuv.data;
		frame->data[0] = pdata;							//fill y
		frame->data[1] = pdata + frame_size;			//fill u
		frame->data[2] = pdata + frame_size * 5 / 4;	//fill v
	}
}
