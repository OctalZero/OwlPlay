#include "OwlDemux.h"
#include <iostream>
using namespace std;
extern "C" {
#include "libavformat/avformat.h"
}
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")

// ���������ֵ
static double r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

bool OwlDemux::Open(const char* url)
{
	Close();
	//��������
	AVDictionary* opts = NULL;
	//����rtsp����tcpЭ���
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);

	//������ʱʱ��
	av_dict_set(&opts, "max_delay", "500", 0);


	mutex_.lock();
	int re = avformat_open_input(
		&ic,
		url,
		0,  // 0��ʾ�Զ�ѡ������
		&opts //�������ã�����rtsp����ʱʱ��
	);
	if (re != 0)
	{
		mutex_.unlock();
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "open " << url << " failed! :" << buf << endl;

		return false;
	}
	cout << "open " << url << " success! " << endl;

	//��ȡ����Ϣ,
	re = avformat_find_stream_info(ic, 0);

	//��ʱ�� ����
	total_ms_ = ic->duration / (AV_TIME_BASE / 1000);
	cout << "total_ms_ = " << total_ms_ << endl;

	//��ӡ��Ƶ����ϸ��Ϣ
	av_dump_format(ic, 0, url, 0);

	// ��ȡ��Ƶ��������һ��ͨ�������������ķ�����ȡ
	video_stream_ = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	AVStream* as = ic->streams[video_stream_];
	// �洢���
	width_ = as->codecpar->width;
	height_ = as->codecpar->height;

	cout << "=================================================" << endl;
	cout << video_stream_ << "��Ƶ��Ϣ" << endl;
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	cout << "width=" << as->codecpar->width << endl;
	cout << "height=" << as->codecpar->height << endl;
	// ֡�� fps ����ת��
	cout << "video fps = " << r2d(as->avg_frame_rate) << endl;

	// ��ȡ��Ƶ��
	audio_stream_ = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	as = ic->streams[audio_stream_];
	sample_rate_ = as->codecpar->sample_rate;
	channels_ = as->codecpar->channels;
	cout << "=================================================" << endl;
	cout << audio_stream_ << "��Ƶ��Ϣ" << endl;
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	cout << "sample_rate = " << as->codecpar->sample_rate << endl;
	//AVSampleFormat;
	cout << "channels = " << as->codecpar->channels << endl;
	//һ֡���ݣ��� ��ͨ�������� 
	cout << "frame_size = " << as->codecpar->frame_size << endl;
	mutex_.unlock();

	return true;
}

AVPacket* OwlDemux::Read()
{
	mutex_.lock();
	if (!ic) {  // �ݴ�
		mutex_.unlock();
		return nullptr;
	}
	AVPacket* pkt = av_packet_alloc();  // �������ռ�
	// ��ȡһ֡�����������ݿռ�
	int re = av_read_frame(ic, pkt);
	if (re != 0) {  // ����0Ϊ�ɹ�
		mutex_.unlock();
		av_packet_free(&pkt);  // �ͷŶ���
		return nullptr;
	}
	// pts\dtsת��Ϊ����, ������ͬ��
	pkt->pts = pkt->pts * (r2d(ic->streams[pkt->stream_index]->time_base) * 1000);
	pkt->dts = pkt->dts * (r2d(ic->streams[pkt->stream_index]->time_base) * 1000);
	mutex_.unlock();
	//cout << pkt->pts << " " << flush;

	return pkt;
}

bool OwlDemux::isAudio(AVPacket* pkt)
{
	if (!pkt) return false;
	if (pkt->stream_index == video_stream_) {
		return false;
	}
	return true;
}

AVCodecParameters* OwlDemux::CopyVideoPara()
{
	mutex_.lock();
	if (!ic) {
		mutex_.unlock();
		return nullptr;
	}
	AVCodecParameters* para = avcodec_parameters_alloc();
	avcodec_parameters_copy(para, ic->streams[video_stream_]->codecpar);

	mutex_.unlock();

	return para;
}

AVCodecParameters* OwlDemux::CopyAudioPara()
{
	mutex_.lock();
	if (!ic) {
		mutex_.unlock();
		return nullptr;
	}
	AVCodecParameters* para = avcodec_parameters_alloc();
	avcodec_parameters_copy(para, ic->streams[audio_stream_]->codecpar);

	mutex_.unlock();

	return para;
}

bool OwlDemux::Seek(double pos)
{
	mutex_.lock();
	if (!ic) {
		mutex_.unlock();
		return false;
	}
	// �����ȡ���壬��ֹճ������
	avformat_flush(ic);

	long long seek_pos = 0;
	// ����һ����ת���������ݴ�Ļ������Կ����������ַ�������ֹduration������
	seek_pos = ic->streams[video_stream_]->duration * pos;
	int re = av_seek_frame(ic, video_stream_, seek_pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
	mutex_.unlock();
	if (re < 0) {  //����>=0Ϊ�ɹ�
		return false;
	}

	return true;
}

void OwlDemux::Clear()
{
	mutex_.lock();
	if (!ic) {
		mutex_.unlock();
		return;
	}
	// �����ȡ���壬��ֹճ������
	avformat_flush(ic);
	mutex_.unlock();
}

void OwlDemux::Close()
{
	mutex_.lock();
	if (!ic) {
		mutex_.unlock();
		return;
	}
	avformat_close_input(&ic);
	// ����ý����ʱ�������룩
	total_ms_ = 0;
	mutex_.unlock();
}

OwlDemux::OwlDemux()
{
	static bool is_first = true;
	static std::mutex demux_mutex;
	demux_mutex.lock();
	if (is_first) {
		//��ʼ����װ��
		av_register_all();

		//��ʼ������� �����Դ�rtsp rtmp http Э�����ý����Ƶ��
		avformat_network_init();
		is_first = false;
	}
	demux_mutex.unlock();
}

OwlDemux::~OwlDemux()
{
}
