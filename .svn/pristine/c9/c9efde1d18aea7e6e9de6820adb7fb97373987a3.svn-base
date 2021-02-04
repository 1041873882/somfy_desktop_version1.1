
#include "sys.h"
#include "MediaPlayer.h"
#include "libswresample/swresample.h"

static void *sys_media_thread(void *)
{
	struct sched_param sp;
	sp.sched_priority = sched_get_priority_max(SCHED_RR)-2;
	pthread_setschedparam(pthread_self(), SCHED_RR, &sp);
	player.process();
	return NULL;
}

MediaPlayer player;

MediaPlayer::MediaPlayer()
{
	m_avformat_ctx = NULL;
	m_avcodec_ctx = NULL;
	m_swr_ctx = NULL;

	m_out_rate = 16000;
	m_out_channels = 1;
	m_out_samples = AV_SAMPLE_FMT_S16;

	m_stop = 0;
	m_cycle = 0;
	sem_init(&m_wait, 0, 0);
	pthread_mutex_init(&m_mutex, NULL);
}

MediaPlayer::~MediaPlayer()
{
}

int MediaPlayer::open(const char *url)
{
	AutoMutex mutex(&m_mutex);
	int error = -1;
	AVCodec *input_codec;

	if (m_avformat_ctx != NULL || url == NULL) {
		return -1;
	}
	if ((error = avformat_open_input(&m_avformat_ctx, url, NULL, NULL)) < 0) {
		fprintf(stderr, "MediaPlayer::open avformat_open_input '%s' (error '%d')\n", url, (error));
		m_avformat_ctx = NULL;
		return -1;
	}
	if ((error = avformat_find_stream_info(m_avformat_ctx, NULL)) < 0) {
		fprintf(stderr, "MediaPlayer::open avformat_find_stream_info(error '%d')\n", (error));
		goto err;
	}
	for (int i=0; i<m_avformat_ctx->nb_streams; i++) {
		AVCodecParameters * paramaters = m_avformat_ctx->streams[i]->codecpar;
		if (paramaters->codec_type == AVMEDIA_TYPE_AUDIO) {
			input_codec = avcodec_find_decoder(m_avformat_ctx->streams[i]->codecpar->codec_id);
			if (!input_codec) {
				goto err;
			}
			if (!(m_avcodec_ctx = avcodec_alloc_context3(input_codec))) {
				goto err;
			}
			if ((error = avcodec_parameters_to_context(m_avcodec_ctx, paramaters)) < 0) {
				goto err;
			}
			if ((error = avcodec_open2(m_avcodec_ctx, input_codec, NULL)) < 0) {
				fprintf(stderr, "Could not open input codec (error '%d')\n", (error));
				goto err;
			}
			m_avcodec_ctx->pkt_timebase = m_avformat_ctx->streams[i]->time_base;
		}
	}
	m_swr_ctx = swr_alloc_set_opts(NULL, av_get_default_channel_layout(m_out_channels), m_out_samples, m_out_rate,
								   av_get_default_channel_layout(m_avcodec_ctx->channels),
								   m_avcodec_ctx->sample_fmt,
								   m_avcodec_ctx->sample_rate,
								   0, NULL);
	if (!m_swr_ctx) {
		fprintf(stderr, "Could not allocate resample context\n");
		goto err;
	}
	if ((error = swr_init(m_swr_ctx)) < 0) {
		fprintf(stderr, "Could not open resample context\n");
		goto err;
	}
	m_dev.set(m_out_rate, m_out_channels, 1);
	if (m_avformat_ctx == NULL) {
		goto err;
	}
	if (m_dev.open()) {
		goto err;
	}
	return 0;
err:
	if (m_avformat_ctx) {
		avformat_close_input(&m_avformat_ctx);
		m_avformat_ctx = NULL;
	}
	if (m_avcodec_ctx) {
		avcodec_free_context(&m_avcodec_ctx);
		m_avcodec_ctx = NULL;
	}
	if (m_swr_ctx) {
		swr_free(&m_swr_ctx);
		m_swr_ctx = NULL;
	}
	return -1;
}

void MediaPlayer::close(void)
{
	m_dev.close();
	AutoMutex mutex(&m_mutex);
	if (m_avcodec_ctx) {
		avcodec_free_context(&m_avcodec_ctx);
		m_avcodec_ctx = NULL;
	}
	if (m_avformat_ctx) {
		avformat_close_input(&m_avformat_ctx);
		m_avformat_ctx = NULL;
	}
	if (m_swr_ctx) {
		swr_free(&m_swr_ctx);
		m_swr_ctx = NULL;
	}
}

void MediaPlayer::start(const char *url, int cycle)
{
	if (url == NULL || m_cycle || m_list.used() >= 3*sizeof(player_list_t)) {
		return;
	}
	static int iThread = 1;
	if (iThread) {
		iThread = 0;
		pthread_t pid;
		if (pthread_create(&pid, NULL, sys_media_thread, NULL) != 0) {
			perror("pthread_create sys_media_thread\n");
		}
	}
	player_list_t list;
	strcpy(list.url, url);
	list.cycle = cycle;
	m_list.put((uint8_t *)&list, sizeof(list));
	sem_post(&m_wait);
}

void MediaPlayer::stop(void)
{
	m_list.flush();
	m_cycle = 0;
	m_stop = 1;
}

int MediaPlayer::decode(void)
{
	AutoMutex mutex(&m_mutex);
	int sz = 0, error;
	AVPacket packet;
	AVFrame *frame = NULL;
	AVCodecParameters *ctx;
	uint8_t *pcm = new uint8_t[192000*2];
	uint8_t *samples[2] = {0};
	samples[0] = (uint8_t *)pcm;

	if (!(frame = av_frame_alloc())) {
		fprintf(stderr, "MediaPlayer::decode av_frame_alloc err!\n");
		return -1;
	}
	av_init_packet(&packet);
	packet.data = NULL;
	packet.size = 0;
	if ((error = av_read_frame(m_avformat_ctx, &packet)) < 0) {
		fprintf(stderr, "Could not read frame(error '%d')\n", error);
		sz = -1;
		goto err;
	}
	ctx = m_avformat_ctx->streams[packet.stream_index]->codecpar;
	if (ctx && ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
		if ((error = avcodec_send_packet(m_avcodec_ctx, &packet)) < 0) {
			fprintf(stderr, "Could not send packet for decoding(error '%d')\n", (error));
			sz = -1;
			goto err;
		}
		while ((error = avcodec_receive_frame(m_avcodec_ctx, frame)) == 0) {
			if (error == AVERROR(EAGAIN) || error == AVERROR_EOF || error < 0) {
				goto err;
			}
			if ((error = swr_convert(m_swr_ctx,
						samples, 192000,
						(const uint8_t **)(frame->extended_data), frame->nb_samples)) < 0) {
				fprintf(stderr, "Could not convert input samples (error '%d')\n", (error));
				goto err;
			}
			m_dev.write(samples[0], error*2);
			sz += error*2;
		}
		m_total += sz;
	}
err:
	av_frame_free(&frame);
	if (pcm) {
		delete[] pcm;
	}
	av_packet_unref(&packet);
	return sz;
}

void MediaPlayer::playing(player_list_t *list)
{
	if (list->cycle) {
		m_list.put((uint8_t *)list, sizeof(player_list_t));
	}
	m_cycle = list->cycle;
	int r = this->open(list->url);
	printf("open r =%d\n", r);
	if (r) {
		this->close();
		m_cycle = 0;
		return;
	}
	//设置音频DAC音量
	OssAudio::volume(0x3F);
	struct timeval ptv;
	uint32_t ss = 2*m_dev.m_rate*m_dev.m_channels;
	gettimeofday(&ptv, NULL);
	m_total = 0;
	while (!m_stop) {
		struct timeval tv;
		fd_set wfds;
		FD_ZERO(&wfds);
		FD_SET(m_dev.fd(), &wfds);
		tv.tv_sec = 0;
		tv.tv_usec = 20*1000;
		select(m_dev.fd()+1, NULL, &wfds, NULL, &tv);
		int dts = (1000*m_total/ss) - __ts(ptv);
		if (dts < 450 && FD_ISSET(m_dev.fd(), &wfds)) {
			int r = this->decode();
			if (r < 0) {
				break;
			}
		} else {
			usleep(20*1000);
		}
	}
	this->close();
	m_cycle = 0;
}

void MediaPlayer::process(void)
{
	//设置静音
	OssAudio::volume(0x00);
	if (!m_dev.open()) {
		uint8_t d[128];
		memset(d, 0, sizeof(d));
		for (int i=0; i<64; i++) {
			m_dev.write(d, sizeof(d));
		}
		m_dev.close();
	}
	while (1) {
		m_stop = 0;
		player_list_t list;
		int r = m_list.get((uint8_t *)&list, sizeof(list));
		if (r > 0) {
			this->playing(&list);
		} else {
			sem_wait(&m_wait);
		}
	}
}
