#include "AudioDeocderFFmpeg.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}
#include "Packet.h"
namespace balsampear
{
	
	AudioDeocderFFmpeg::AudioDeocderFFmpeg()
	{
		
		avframe_ = av_frame_alloc();
	}

	AudioDeocderFFmpeg::~AudioDeocderFFmpeg()
	{
		av_frame_free(&avframe_);
	}

	bool AudioDeocderFFmpeg::decode(const Packet& pkt)
	{
		if (codecCtx_)
		{
			int ret = avcodec_send_packet(codecCtx_, pkt.asAVPacket()); 
			if (ret)
				return false;//Error decoding video frame
			ret = avcodec_receive_frame(codecCtx_, avframe_);

			//ǰ��֡�᷵��AVERROR(EAGAIN)
			if (ret)
				return false;

			

			
			return true;
		}
		return false;
	}
	SwrContext* mpSwrCtx = nullptr;
	balsampear::VideoFrame AudioDeocderFFmpeg::frame()
	{
		shared_ptr<Frame::Content> content = std::make_shared<Frame::Content>();
		
		VideoFrame f(avframe_->width, avframe_->height, VideoFormat(), content);

		int test = av_frame_get_best_effort_timestamp(avframe_);
		int size = avframe_->nb_samples * av_get_bytes_per_sample(codecCtx_->sample_fmt) * avframe_->channels;
		int audioLen = av_samples_get_buffer_size(NULL, 2, avframe_->nb_samples, codecCtx_->sample_fmt, 1);
		double sec = (double)audioLen / (double)(2 * codecCtx_->channels * codecCtx_->sample_rate);

		//content->data_.resize(8192);
		//memcpy(&content->data_[0], avframe_->data[0], 8192);
		if (!mpSwrCtx)
		{
			mpSwrCtx = swr_alloc();
			mpSwrCtx = swr_alloc_set_opts(mpSwrCtx,
				av_get_default_channel_layout(codecCtx_->channels),
				AV_SAMPLE_FMT_U8,
				codecCtx_->sample_rate,
				av_get_default_channel_layout(codecCtx_->channels),
				codecCtx_->sample_fmt,
				codecCtx_->sample_rate,
				0,
				NULL
			);
		}
		int bufferSize = av_samples_get_buffer_size(NULL, 2, avframe_->nb_samples, AV_SAMPLE_FMT_U8, 1);
		content->data_.resize(bufferSize);
		Byte* p = &content->data_[0];
		swr_init(mpSwrCtx);
		int convertSize = swr_convert(mpSwrCtx, &p,
			avframe_->nb_samples,
			(const uint8_t**)avframe_->extended_data, avframe_->nb_samples);

		return f;
	}

}