#include "VideoDecoderFFmpeg.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}
#include "Packet.h"
namespace balsampear
{
	VideoDecoderFFmpeg::VideoDecoderFFmpeg()
		:avframe_(nullptr)
	{
		avframe_ = av_frame_alloc();
	}

	VideoDecoderFFmpeg::~VideoDecoderFFmpeg()
	{
		av_frame_free(&avframe_);
	}

	bool VideoDecoderFFmpeg::decode(const Packet& pkt)
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

			if (avframe_->width <= 0 || avframe_->height <= 0)
				return false;
			
			return true;
		}
		return false;
	}

	VideoFrame VideoDecoderFFmpeg::frame()
	{
		if (avframe_->width <= 0 || avframe_->height <=0 || !codecCtx_)
			return VideoFrame();

		shared_ptr<Frame::Content> content = std::make_shared<Frame::Content>();
		content->data_.resize((size_t)avframe_->width * avframe_->height * 3 / 2);
		VideoFrame f(avframe_->width, avframe_->height, codecCtx_->pix_fmt, content);

		size_t ySize = (size_t)avframe_->width * avframe_->height;
		memcpy(&content->data_[0], avframe_->data[0], ySize);
		memcpy(&content->data_[ySize], avframe_->data[1], ySize / 4);
		memcpy(&content->data_[ySize * 5 / 4], avframe_->data[2], ySize / 4);

		return f;
	}

}


