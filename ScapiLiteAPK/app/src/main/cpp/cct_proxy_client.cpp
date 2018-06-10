
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <memory.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include <string>
#include <vector>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/uio.h>

#ifdef __ANDROID__

#include <android/log.h>

#define lc_fatal(...) __android_log_print(ANDROID_LOG_FATAL,m_logcat.c_str(),__VA_ARGS__)
#define lc_error(...) __android_log_print(ANDROID_LOG_ERROR,m_logcat.c_str(),__VA_ARGS__)
#define lc_warn(...) __android_log_print(ANDROID_LOG_WARN,m_logcat.c_str(),__VA_ARGS__)
#define lc_notice(...) __android_log_print(ANDROID_LOG_INFO,m_logcat.c_str(),__VA_ARGS__)
#define lc_info(...) __android_log_print(ANDROID_LOG_INFO,m_logcat.c_str(),__VA_ARGS__)
#define lc_debug(...) __android_log_print(ANDROID_LOG_DEBUG,m_logcat.c_str(),__VA_ARGS__)

#else

#include <log4cpp/Category.hh>

#define lc_fatal(...) log4cpp::Category::getInstance(m_logcat).error(__VA_ARGS__)
#define lc_error(...) log4cpp::Category::getInstance(m_logcat).error(__VA_ARGS__)
#define lc_warn(...) log4cpp::Category::getInstance(m_logcat).warn(__VA_ARGS__)
#define lc_notice(...) log4cpp::Category::getInstance(m_logcat).notice(__VA_ARGS__)
#define lc_info(...) log4cpp::Category::getInstance(m_logcat).info(__VA_ARGS__)
#define lc_debug(...) log4cpp::Category::getInstance(m_logcat).debug(__VA_ARGS__)

#endif

#include <event2/event.h>

#include "comm_client.h"
#include "cct_proxy_client.h"
#include "cc_proxy_protocol.h"
#include "comm_client_cb_api.h"

#define PPRDFD	0
#define PPWRFD	1

static const struct timeval naught = {0,0};
static const struct timeval _50ms_ = {0,50000};
static const struct timeval _1sec_ = {1,0};

cct_proxy_client::cct_proxy_client(cc_args_t * cc_args)
: comm_client(cc_args), m_proxy_addr(cc_args->proxy_addr), m_proxy_port(cc_args->proxy_port), m_peer_mask(NULL), m_mask_size(0)
, m_sockfd(-1), m_base(NULL), m_timer(NULL), m_read(NULL), m_write(NULL)
{
}

cct_proxy_client::~cct_proxy_client()
{
}

void cct_proxy_client::set_proxy_service(const char * proxy_addr, const u_int16_t proxy_port)
{
	m_proxy_addr = proxy_addr;
	m_proxy_port = proxy_port;
}

int cct_proxy_client::start(const unsigned int id, const unsigned int peer_count, const char * comm_conf_file, comm_client_cb_api * sink)
{
	m_mask_size = (peer_count + 7) / 8;
	m_peer_mask = new u_int8_t[m_mask_size];
	memset(m_peer_mask, 0, m_mask_size);

	return comm_client::start(id, peer_count, comm_conf_file, sink);
}

void cct_proxy_client::stop()
{
	comm_client::stop();

	delete m_peer_mask;
	m_peer_mask = NULL;
	m_mask_size = 0;
}

void cct_proxy_client::run()
{
	m_base = event_base_new();
	if(NULL != m_base)
	{
		m_read = event_new(m_base, -1, EV_TIMEOUT, connect_cb, this);
		if(NULL != m_read)
		{
			if(0 == event_add(m_read, &naught))
			{
				if(0 == pipe(m_out_pipe))
				{
					m_timer = event_new(m_base, -1, EV_TIMEOUT|EV_PERSIST, timer_cb, this);
					if(NULL != m_timer)
					{
						if(0 == event_add(m_timer, &_50ms_))
						{
							lc_notice("%s: starting event loop.", __FUNCTION__);
							event_base_dispatch(m_base);
							lc_notice("%s: event loop stopped.", __FUNCTION__);

							if(NULL != m_write)
							{
								event_del(m_write);
								event_free(m_write);
								m_write = NULL;
							}

							if(-1 != m_sockfd)
							{
								close(m_sockfd);
								m_sockfd = -1;
							}
							event_del(m_timer);
						}
						else
							lc_error("%s: timer event addition failed.", __FUNCTION__);

						event_free(m_timer);
						m_timer = NULL;
					}
					else
						lc_error("%s: timer event allocation failed.", __FUNCTION__);

					close(m_out_pipe[PPRDFD]);
					m_out_pipe[PPRDFD] = -1;
					close(m_out_pipe[PPWRFD]);
					m_out_pipe[PPWRFD] = -1;
				}
				else
				{
		            int errcode = errno;
		            char errmsg[256];
		            lc_error("%s: pipe() failed with error %d : [%s].", __FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
				}

				event_del(m_read);
			}
			else
				lc_error("%s: connect event addition failed.", __FUNCTION__);

			event_free(m_read);
			m_read = NULL;
		}
		else
			lc_error("%s: connect event allocation failed.", __FUNCTION__);
		event_base_free(m_base);
		m_base = NULL;
	}
	else
		lc_error("%s: event base allocation failed.", __FUNCTION__);
}

int cct_proxy_client::send(const unsigned int dst_id, const unsigned char * msg, const size_t size)
{
	size_t offset = dst_id/8;
	u_int8_t mask = 0x01 << (dst_id%8);
	if(0 == (m_peer_mask[offset] & mask))
	{
		lc_warn("%s: rejected a message to disconnected peer %u.", __FUNCTION__, dst_id);
		return -1;
	}

	proxy_msg_t pm;
	pm.type = MSG_TYPE_PMS;
	pm.id = dst_id;
	pm.param = size;
	pm.hton();

	struct iovec iov[2];
	iov[0].iov_base = &pm;
	iov[0].iov_len = sizeof(proxy_msg_t);
	iov[1].iov_base = (void *)msg;
	iov[1].iov_len = size;

	ssize_t nwrit = writev(m_out_pipe[PPWRFD], iov, 2);
	if((ssize_t)(sizeof(proxy_msg_t) + size) != nwrit)
	{
		if(0 > nwrit)
		{
			int errcode = errno;
			char errmsg[512];
			lc_error("%s: writev() failed with error %d : [%s].", __FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
	        exit(-__LINE__);
		}
		else
		{
			lc_error("%s: writev() partial write of %lu out of %lu.", __FUNCTION__, (size_t)nwrit, (sizeof(proxy_msg_t) + size));
	        exit(-__LINE__);
		}
	}
	else
		lc_debug("%s: %lu bytes written to out pipe.", __FUNCTION__, (size_t)nwrit);
	return 0;
}

void cct_proxy_client::on_connect()
{
	if(0 == make_connection())
	{
		event_free(m_read);
		m_data.clear();

		m_read = event_new(m_base, m_sockfd, EV_READ|EV_PERSIST, read_cb, this);
		if(0 != event_add(m_read, NULL))
		{
			lc_error("%s: read event addition failed.", __FUNCTION__);
			exit(-__LINE__);
		}

		m_write = event_new(m_base, m_out_pipe[PPRDFD], EV_READ, write1_cb, this);
		if(0 != event_add(m_write, NULL))
		{
			lc_error("%s: write event addition failed.", __FUNCTION__);
			exit(-__LINE__);
		}
	}
	else
	{
		if(0 != event_add(m_read, &_1sec_))
		{
			lc_error("%s: connect event addition failed.", __FUNCTION__);
			exit(-__LINE__);
		}
	}
}

void cct_proxy_client::on_timer()
{
	if(!get_run_flag())
	{
		lc_debug("%s: run flag down; breaking event loop.", __FUNCTION__);
		event_base_loopbreak(m_base);
	}
}

void cct_proxy_client::on_read()
{
	u_int8_t buffer[4096];
	ssize_t nread = read(m_sockfd, buffer, 4096);
	if(0 < nread)
	{
		lc_debug("%s: read %ld bytes.", __FUNCTION__, nread);
		m_data.insert(m_data.end(), buffer, buffer + nread);
		process_messages();
		return;
	}
	else if(0 == nread)
		lc_notice("%s: read 0 bytes; disconnection.", __FUNCTION__);
	else
	{
		int errcode = errno;
		char errmsg[512];
        lc_error("%s: read() failed with error %d : [%s].", __FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
	}

	m_data.clear();
	close(m_sockfd);
	m_sockfd = -1;

	event_del(m_write);
	event_free(m_write);
	m_write = NULL;

	event_del(m_read);
	event_free(m_read);
	m_read = event_new(m_base, -1, EV_TIMEOUT, connect_cb, this);
	if(NULL == m_read)
	{
		lc_error("%s: read event allocation failed.", __FUNCTION__);
		exit(-__LINE__);
	}
	if(0 != event_add(m_read, &_1sec_))
	{
		lc_error("%s: connect event addition failed.", __FUNCTION__);
		exit(-__LINE__);
	}
}

void cct_proxy_client::on_write1()
{
	event_free(m_write);
	m_write = event_new(m_base, m_sockfd, EV_WRITE, write2_cb, this);
	if(NULL == m_write)
	{
		lc_error("%s: write event allocation failed.", __FUNCTION__);
		exit(-__LINE__);
	}
	if(0 != event_add(m_write, NULL))
	{
		lc_error("%s: write event addition failed.", __FUNCTION__);
		exit(-__LINE__);
	}
}

void cct_proxy_client::on_write2()
{
	event_free(m_write);
	m_write = NULL;

	int result = splice(m_out_pipe[PPRDFD], NULL, m_sockfd, NULL, 4096, 0);
	if(0 > result)
	{
		int errcode = errno;
		char errmsg[512];
        lc_error("%s: splice() failed with error %d : [%s].", __FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
    	lc_info("%s: disconnecting.", __FUNCTION__);

    	m_data.clear();
    	close(m_sockfd);
    	m_sockfd = -1;

    	event_del(m_read);
    	event_free(m_read);
    	m_read = event_new(m_base, -1, EV_TIMEOUT, connect_cb, this);
    	if(NULL == m_read)
    	{
    		lc_error("%s: read event allocation failed.", __FUNCTION__);
    		exit(-__LINE__);
    	}
    	if(0 != event_add(m_read, &_1sec_))
    	{
    		lc_error("%s: connect event addition failed.", __FUNCTION__);
    		exit(-__LINE__);
    	}
	}
	else
	{
		lc_debug("%s: %d bytes spliced out to conn.", __FUNCTION__, result);

		m_write = event_new(m_base, m_out_pipe[PPRDFD], EV_READ, write1_cb, this);
		if(NULL == m_write)
		{
			lc_error("%s: write event allocation failed.", __FUNCTION__);
			exit(-__LINE__);
		}
		if(0 != event_add(m_write, NULL))
		{
			lc_error("%s: write event addition failed.", __FUNCTION__);
			exit(-__LINE__);
		}
	}
}

int cct_proxy_client::make_connection()
{
	if (0 <= (m_sockfd = socket(AF_INET, SOCK_STREAM, 0)))
	{
		lc_debug("%s: socket created %d.", __FUNCTION__, m_sockfd);

		struct sockaddr_in sockaddr;
		if (0 != inet_aton(m_proxy_addr.c_str(), &sockaddr.sin_addr))
	    {
			sockaddr.sin_port = htons(m_proxy_port);
			sockaddr.sin_family = AF_INET;

			if(0 == connect(m_sockfd, (const struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in)))
			{
				lc_notice("%s: socket %d connected to [%s:%hu].", __FUNCTION__, m_sockfd, m_proxy_addr.c_str(), m_proxy_port);

				proxy_msg_t cdm;
				ssize_t nread = read(m_sockfd, &cdm, sizeof(proxy_msg_t));
				if((ssize_t)sizeof(proxy_msg_t) == nread)
				{
					cdm.ntoh();
					if(MSG_TYPE_CDM == cdm.type)
					{
						lc_notice("%s: client details from proxy service: id=%u; count=%u;.", __FUNCTION__, cdm.id, cdm.param);
						if(cdm.id == this->m_id && cdm.param == this->m_peer_count)
							return 0;
						else
							lc_error("%s: proxy service client details mismatch.", __FUNCTION__);
					}
					else
						lc_error("%s: read message is not a client details message; type = %u.", __FUNCTION__, cdm.type);
				}
				else if(0 > nread)
				{
					int errcode = errno;
					char errmsg[512];
			        lc_error("%s: read() failed with error %d : [%s].", __FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
				}
				else
					lc_error("%s: failure reading client details message on connection; %ld bytes read.", __FUNCTION__, nread);
			}
			else
			{
				int errcode = errno;
				char errmsg[512];
		        lc_error("%s: connect() failed with error %d : [%s].", __FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
			}
	    }
		else
			lc_error("%s: failure converting proxy IP address <%s>.", __FUNCTION__, m_proxy_addr.c_str());

		close(m_sockfd);
		m_sockfd = -1;
	}
	else
	{
		int errcode = errno;
		char errmsg[512];
        lc_error("%s: socket() failed with error %d : [%s].", __FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
	}
	return -1;
}

int cct_proxy_client::process_messages()
{
	while(!m_data.empty())
	{
		size_t data_size = m_data.size();
		if(sizeof(proxy_msg_t) > data_size)
			return 0;

		proxy_msg_t hdr = *((proxy_msg_t *)m_data.data());
		hdr.ntoh();

		switch(hdr.type)
		{
		case MSG_TYPE_PCU:
			if(0 == hdr.param)
			{
				size_t offset = hdr.id/8;
				u_int8_t mask = 0x01 << (hdr.id%8);
				m_peer_mask[offset] &= ~(mask);
				this->m_sink->on_comm_down_with_party(hdr.id);
			}
			else
			{
				size_t offset = hdr.id/8;
				u_int8_t mask = 0x01 << (hdr.id%8);
				m_peer_mask[offset] |= mask;
				this->m_sink->on_comm_up_with_party(hdr.id);
			}
			m_data.erase(m_data.begin(), m_data.begin() + sizeof(proxy_msg_t));
			break;
		case MSG_TYPE_PMS:
			if((sizeof(proxy_msg_t) + hdr.param) > data_size)
				return 0;
			else
			{
				this->m_sink->on_comm_message(hdr.id, m_data.data() + sizeof(proxy_msg_t), hdr.param);
				m_data.erase(m_data.begin(), m_data.begin() + (sizeof(proxy_msg_t) + hdr.param));
			}
			break;
		default:
			lc_error("%s: invalid message type %u.", __FUNCTION__, hdr.type);
			exit(-__LINE__);
		}
	}
	return 0;
}

void cct_proxy_client::connect_cb(evutil_socket_t fd, short what, void * arg)
{
	((cct_proxy_client *)arg)->on_connect();
}

void cct_proxy_client::timer_cb(evutil_socket_t fd, short what, void * arg)
{
	((cct_proxy_client *)arg)->on_timer();
}

void cct_proxy_client::read_cb(evutil_socket_t fd, short what, void * arg)
{
	((cct_proxy_client *)arg)->on_read();
}

void cct_proxy_client::write1_cb(evutil_socket_t fd, short what, void * arg)
{
	((cct_proxy_client *)arg)->on_write1();
}

void cct_proxy_client::write2_cb(evutil_socket_t fd, short what, void * arg)
{
	((cct_proxy_client *)arg)->on_write2();
}
