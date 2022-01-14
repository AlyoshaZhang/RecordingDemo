#include "ffmpegConfig.h"
#include <stdio.h>

int rtmp_() {
	AVOutputFormat* ofmt = NULL;
	AVFormatContext* ifmt_ctx = NULL, * ofmt_ctx = NULL;
	AVPacket* pkt;
	const char* in_filename, * out_filename;
	int ret, i;
	int videoindex = 1;
	int frame_index = 0;
	int64_t start_time = 0;

	in_filename = "in.h264";
	//in_filename = "out.flv";
	//in_filename = "out.mp4";

	out_filename = "rtmp://localhost/publishlive/livestream";//输出 URL (output URL)

	av_register_all();
	avformat_network_init();
	//输入（Input)
	if (ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) {
		printf("Failed to retrieve input stream information");
		goto end;
	}
	if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
		printf("Failed to retrieve input stream information");
		goto end;
	}

	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}
	}

	av_dump_format(ifmt_ctx, 0, in_filename, 0);

	//输出(Output)
	avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", out_filename);	//RTMP

	if (!ofmt_ctx) {
		printf("Could not create output context\n");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	ofmt = ofmt_ctx->oformat;
	for (i = 0; i < ifmt_ctx->nb_streams; i++) {
		//根据输入流创建输出流(Create output AVStream according to input AVStream)
		AVStream* in_stream = ifmt_ctx->streams[i];
		AVStream* out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
		if (!out_stream) {
			printf("Failed allocating output stream\n");
			ret = AVERROR_UNKNOWN;
			goto end;
		}
		//复制AVCodecContext的设置(Copy the settings of AVCodecContext);
		ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
		if (ret < 0) {
			printf("Failed to copy context from input to output stream codec context\n");
			goto end;
		}
		out_stream->codec->codec_tag = 0;
		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
			out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}
	//Dump Format
	av_dump_format(ofmt_ctx, 0, out_filename, 1);
	//打开输出 URL (Open output URL)
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
		if (ret < 0) {
			printf("Could not open output URL '%s'\n", out_filename);
			goto end;
		}
		//写文件头(Write file header)
		ret = avformat_write_header(ofmt_ctx, NULL);
		if (ret < 0) {
			printf("Could not open output URL '%s'\n", out_filename);
			goto end;
		}
	}



end:


}
