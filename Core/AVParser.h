#pragma once
#include "Base/StringPiece.h"
#include "AudioFormat.h"
#include "VideoFormat.h"
#include <memory>

using std::shared_ptr;
struct AVFormatContext;
struct AVCodecContext;

using uint64 = unsigned long long;
namespace balsampear
{
	class AVDemuxer;
	class AudioDecoder;
	class VideoDecoder;
	enum class MediaType:int
	{
		Type_Invalid = 0,
		Type_Audio = 1,
		Type_Video = 2,
	};
	class AVParser
	{
	public:
		AVParser();
		AVParser(const StringPiece& file);
		AVParser(const AVParser&) = delete;
		AVParser& operator= (const AVParser&) = delete;
		~AVParser();
		void setFile(const StringPiece& file);
		StringPiece currentFile();
		bool parse();
		AVFormatContext* getFormatCtx();
		AVCodecContext* getAudioCodecCtx();
		AVCodecContext* getVideoCodexCtx();
		double duration();
		AudioFormat getAudioFormat();
		VideoFormat getVideoFormat();
		int getMediaType();
		double videotimebase();
		double audeiotimebase();
		int framerate();

		int audioStream();
		int videoStream();
		

	private:
		StringPiece file_;
		AVFormatContext *formatCtx_; 
		int type_;
		double duration_;
		double videoTimeBase_;
		double audioTimeBase_;
		int framerate_;
		AVCodecContext *aCodecCtx_;
		int aIndexOfStream_;
		AudioFormat aformat_;
		AVCodecContext *vCodecCtx_;
		int vIndexOfStream_;
		VideoFormat vformat_;
	};
}


