
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>

#include <string>
#include <vector>
#include <map>
#include <set>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <android/log.h>

#define APPNAME "ScapiLite"

#define lc_fatal(...) __android_log_print(ANDROID_LOG_FATAL,APPNAME,__VA_ARGS__)
#define lc_error(...) __android_log_print(ANDROID_LOG_ERROR,APPNAME,__VA_ARGS__)
#define lc_warn(...) __android_log_print(ANDROID_LOG_WARN,APPNAME,__VA_ARGS__)
#define lc_notice(...) __android_log_print(ANDROID_LOG_INFO,APPNAME,__VA_ARGS__)
#define lc_info(...) __android_log_print(ANDROID_LOG_INFO,APPNAME,__VA_ARGS__)
#define lc_debug(...) __android_log_print(ANDROID_LOG_DEBUG,APPNAME,__VA_ARGS__)

#include <event2/event.h>

#include "comm_client.h"
#include "comm_client_tcp_mesh.h"
#include "comm_client_cb_api.h"

/*"Data can be written to the file descriptor fildes[1] and read from the file descriptor fildes[0]."*/
#define PIPE_READ_FD	0
#define PIPE_WRITE_FD	1

static const struct timeval zeroto = {0,0};
static const struct timeval asec = {1,0};

comm_client_tcp_mesh::comm_client_tcp_mesh(comm_client::cc_args_t * args)
: comm_client(args), the_base(NULL)
{
}

comm_client_tcp_mesh::~comm_client_tcp_mesh()
{
}

int comm_client_tcp_mesh::start(const unsigned int id, const unsigned int peer_count, const char * comm_conf_file, comm_client_cb_api * sink)
{
	lc_debug("%s: id=%u; count=%u; file=%s; ", __FUNCTION__, id, peer_count, comm_conf_file);

	if(id >= peer_count)
	{
		lc_error("%s: invalid id/parties values %u/%u", __FUNCTION__, id, peer_count);
		return -1;
	}

	if(get_run_flag())
	{
		lc_error("%s: this comm client is already started", __FUNCTION__);
		return -1;
	}
	set_run_flag(true);

	m_id = id;
	m_peer_count = peer_count;
	m_comm_conf_file = comm_conf_file;
	m_sink = sink;

	if(0 != insure_resource_limits())
	{
		lc_error("%s: failed to insure adequate resource limits.", __FUNCTION__);
		return -1;
	}

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	lc_notice("%s: started %lu.%03lu", __FUNCTION__, ts.tv_sec, ts.tv_nsec/1000000);

	if(0 != load_peers(m_peer_count))
	{
		lc_error("%s: parties load failure.", __FUNCTION__);
		return -1;
	}
	lc_debug("%s: %lu peers loaded.", __FUNCTION__, m_peers.size());
	for(size_t i = 0; i < m_peers.size(); ++i)
	{
		lc_debug("%s: peer %u <%s:%hu>", __FUNCTION__, m_peers[i].id, m_peers[i].ip.c_str(), m_peers[i].port);
	}

	if(0 != start_service())
	{
		lc_error("%s: service start failure.", __FUNCTION__);
		return -1;
	}

	return launch();
}

void comm_client_tcp_mesh::stop()
{
	comm_client::stop();
	clear_peers();
}

int comm_client_tcp_mesh::send(const unsigned int dst_id, const unsigned char * msg, const size_t size)
{
	lc_debug("%s: dst_id=%u; size=%lu;", __FUNCTION__, dst_id, size);

	if(!get_run_flag())
	{
		lc_error("%s: comm client is not running.", __FUNCTION__);
		return -1;
	}

	if(-1 != m_peers[dst_id].out_pipe[PIPE_WRITE_FD])
	{
		ssize_t nwrit = write(m_peers[dst_id].out_pipe[PIPE_WRITE_FD], msg, size);
		if(0 > nwrit)
		{
	        int errcode = errno;
	        char errmsg[256];
	        lc_error("%s: write() failed with error %d : [%s].",
	        		__FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
	        return -1;
		}
		else if((size_t)nwrit != size)
		{
	        lc_warn("%s: write() written %lu out of %lu bytes.", __FUNCTION__, (size_t)nwrit, size);
		}
		return 0;
	}
	else
	{
		lc_error("%s: destination id comm is down.", __FUNCTION__);
		return -1;
	}
}

void comm_client_tcp_mesh::run()
{
	the_base = event_base_new();

	if(0 != set_accept())
	{
		lc_error("%s: acceptor event addition failure.", __FUNCTION__);
		return;
	}

	if(0 != add_connectors())
	{
		lc_error("%s: connector events addition failure.", __FUNCTION__);
		return;
	}

	struct event * timer = event_new(the_base, -1, EV_TIMEOUT|EV_PERSIST, comm_client_tcp_mesh::timer_cb, this);
	if(0 != event_add(timer, &asec))
	{
		lc_error("%s: timer event addition failure.", __FUNCTION__);
		event_free(timer);
		return;
	}

	lc_notice("%s: starting event loop.", __FUNCTION__);
	event_base_dispatch(the_base);
	lc_notice("%s: event loop stopped.", __FUNCTION__);

	event_del(timer);
	event_free(timer);
	event_base_free(the_base);
}

void comm_client_tcp_mesh::clear_peers()
{
	for(std::vector< peer_t >::iterator i = m_peers.begin(); i != m_peers.end(); ++i)
	{
		if(-1 != i->sockfd) { close(i->sockfd); i->sockfd = -1; }
		if(-1 != i->out_pipe[PIPE_READ_FD]) { close(i->out_pipe[PIPE_READ_FD]); i->out_pipe[PIPE_READ_FD] = -1; }
		if(-1 != i->out_pipe[PIPE_WRITE_FD]) { close(i->out_pipe[PIPE_WRITE_FD]); i->out_pipe[PIPE_WRITE_FD] = -1; }
		if(NULL != i->reader) { event_del(i->reader); event_free(i->reader); i->reader = NULL; }
		if(NULL != i->writer) { event_del(i->writer); event_free(i->writer); i->writer = NULL; }
	}
	m_peers.clear();
}

int comm_client_tcp_mesh::set_accept()
{
	lc_debug("%s: ", __FUNCTION__);

	m_peers[m_id].reader = event_new(the_base, m_peers[m_id].sockfd, EV_READ, comm_client_tcp_mesh::accept_cb, this);
	if(0 != event_add(m_peers[m_id].reader, NULL))
	{
		lc_error("%s: acceptor addition failure.", __FUNCTION__);
		event_free(m_peers[m_id].reader);
		m_peers[m_id].reader = NULL;
		return -1;
	}
	return 0;
}

int comm_client_tcp_mesh::add_connectors()
{
	for(unsigned int idx = 0; idx < m_id; ++idx)
	{
		if(0 != add_peer_connector(idx, zeroto))
		{
			lc_error("%s: peer %u connector addition failure.", __FUNCTION__, idx);
			return -1;
		}
	}
	return 0;
}

int comm_client_tcp_mesh::add_peer_connector(const unsigned int id, const struct timeval & to)
{
	m_peers[id].reader = event_new(the_base, (int)id, EV_TIMEOUT, comm_client_tcp_mesh::connect_cb, this);
	if(0 != event_add(m_peers[id].reader, &to))
	{
		event_free(m_peers[id].reader);
		m_peers[id].reader = NULL;
		return -1;
	}
	return 0;
}

int comm_client_tcp_mesh::load_peers(const unsigned int peer_count)
{
	lc_debug("%s: count=%u;", __FUNCTION__, peer_count);

	unsigned int n = 0;
	FILE * pf = fopen(m_comm_conf_file.c_str(), "r");
	if(NULL != pf)
	{
		m_peers.resize(peer_count);
		char buffer[128];
		for(n = 0; n < peer_count; ++n)
		{
			bool peer_n_added = false;
			if(NULL != fgets(buffer, 128, pf))
			{
				if(0 == parse_address(buffer, m_peers[n].ip, m_peers[n].port, m_peers[n].inet_addr))
				{
					m_peers[n].client = this;
					m_peers[n].id = n;
					m_peers[n].sockfd = m_peers[n].out_pipe[PIPE_READ_FD] = m_peers[n].out_pipe[PIPE_WRITE_FD] = -1;
					m_peers[n].reader = m_peers[n].writer = NULL;
					peer_n_added = true;
				}
			}
			if(!peer_n_added)
				break;
		}
		fclose(pf);
	}
	return (peer_count == n)? 0: -1;
}

int comm_client_tcp_mesh::insure_resource_limits()
{
	rlim_t required_fds = 10 + 3 * m_peer_count;
	struct rlimit fd_limit;
	if(0 == getrlimit(RLIMIT_NOFILE, &fd_limit))
	{
		if(fd_limit.rlim_max > required_fds)
		{
			lc_info("%s: file descriptors hard limit = %lu / required = %lu.",
					__FUNCTION__, fd_limit.rlim_max, required_fds);

			if(fd_limit.rlim_cur > required_fds)
			{
				lc_info("%s: file descriptors soft limit = %lu / required = %lu.",
						__FUNCTION__, fd_limit.rlim_cur, required_fds);
			}
			else
			{
				lc_info("%s: file descriptors soft limit = %lu / required = %lu; trying to raise the limit.",
						__FUNCTION__, fd_limit.rlim_cur, required_fds);

				fd_limit.rlim_cur = required_fds;
				if(0 == setrlimit(RLIMIT_NOFILE, &fd_limit))
				{
					lc_info("%s: file descriptors soft limit successfully set to %lu.",
							__FUNCTION__, fd_limit.rlim_cur);
				}
				else
				{
					lc_error("%s: file descriptors soft limit failed to be set to %lu.",
							__FUNCTION__, fd_limit.rlim_cur);
					return -1;
				}
			}
		}
		else
		{
			lc_error("%s: not enough available file descriptors - hard limit = %lu / required = %lu.",
					 __FUNCTION__, fd_limit.rlim_max, required_fds);
			return -1;
		}
	}
	else
    {
        int errcode = errno;
        char errmsg[256];
        lc_error("%s: getrlimit() failed with error %d : [%s].",
        		__FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
        return -1;
    }
	return 0;
}

int comm_client_tcp_mesh::start_service()
{
	lc_debug("%s: ", __FUNCTION__);

	peer_t & self(m_peers[m_id]);
	if (0 > (self.sockfd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        int errcode = errno;
        char errmsg[256];
        lc_error("%s: socket() failed with error %d : [%s].",
        		__FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
        return -1;
    }
	lc_debug("%s: self service socket created; fd = %d.", __FUNCTION__, self.sockfd);

	int socket_address_reuse = 1;
	if(0 != setsockopt(self.sockfd, SOL_SOCKET, SO_REUSEADDR, &socket_address_reuse, sizeof(int)))
	{
        int errcode = errno;
        char errmsg[256];
        lc_error("%s: setsockopt(SO_REUSEADDR) failed with error %d : [%s].",
        		__FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
        return -1;
	}
	lc_debug("%s: self service socket address reuse option set.", __FUNCTION__);

	//override the file configured address with addr-any
	struct sockaddr_in my_inet_addr = self.inet_addr;
	my_inet_addr.sin_addr.s_addr = INADDR_ANY;

	if (0 != bind(self.sockfd, (const sockaddr *)&my_inet_addr, (socklen_t)sizeof(struct sockaddr_in)))
	{
        int errcode = errno;
        char errmsg[256];
        lc_error("%s: bind() to [%s:%hu] failed with error %d : [%s].",
        		__FUNCTION__, self.ip.c_str(), self.port, errcode, strerror_r(errcode, errmsg, 256));
        close(self.sockfd);
        self.sockfd = -1;
        return -1;
	}
	lc_debug("%s: socket bound to address %s:%hu.", __FUNCTION__, self.ip.c_str(), self.port);

	if (0 != listen(self.sockfd, this->m_peer_count))
	{
        int errcode = errno;
        char errmsg[256];
        lc_error("%s: listen() on [%s:%hu] failed with error %d : [%s].",
        		__FUNCTION__, self.ip.c_str(), self.port, errcode, strerror_r(errcode, errmsg, 256));
        close(self.sockfd);
        self.sockfd = -1;
        return -1;
	}
	lc_debug("%s: service socket is listening.", __FUNCTION__);

	return 0;
}

void comm_client_tcp_mesh::on_timer(int, short, void *)
{
	if(!get_run_flag())
	{
		lc_debug("%s: run flag down; breaking event loop.", __FUNCTION__);
		event_base_loopbreak(the_base);
	}
}

void comm_client_tcp_mesh::on_accept(int, short, void *)
{
	event_free(m_peers[m_id].reader);
	m_peers[m_id].reader = NULL;

	struct sockaddr_in conn_addr;
	socklen_t conn_addr_len = 0;
	int conn_fd = accept(m_peers[m_id].sockfd, (struct sockaddr *)&conn_addr, &conn_addr_len);
	if(0 <= conn_fd)
	{
		//TODO: possibly trace the source address of the accepted conn: conn_addr/conn_addr_len

		static const struct timeval select_timeout = {10,0};
		m_peers[m_id].reader = event_new(the_base, conn_fd, EV_READ, comm_client_tcp_mesh::select_cb, this);
		if(0 == event_add(m_peers[m_id].reader, &select_timeout))
		{
			lc_info("%s: accepted conn fd %d; waiting to select it.", __FUNCTION__, conn_fd);
			return;
		}
		else
			lc_error("%s: selector addition failed; closing accepted conn %d.", __FUNCTION__, conn_fd);
		close(conn_fd);
	}
	else
	{
        int errcode = errno;
        char errmsg[256];
        lc_error("%s: accept() failed with error %d : [%s].",
        		__FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
	}
    set_accept();
    return;
}

void comm_client_tcp_mesh::on_connect(int fd, short, void *)
{
	const unsigned int id = (const unsigned int)fd;
	lc_debug("%s: id=%u; dst=[%s:%hu]", __FUNCTION__, id, m_peers[id].ip.c_str(), m_peers[id].port);
	event_free(m_peers[id].reader);
	m_peers[id].reader = NULL;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(0 <= sockfd)
	{
		lc_debug("%s: socket created %d.",	__FUNCTION__, sockfd);

		if(0 == connect(sockfd, (const struct sockaddr *)(&m_peers[id].inet_addr), sizeof(struct sockaddr_in)))
		{
			lc_info("%s: successfully connected to peer %u using fd %d", __FUNCTION__, id, sockfd);
			u_int32_t noid = htonl(m_id);
			ssize_t nwrit = write(sockfd, &noid, sizeof(u_int32_t));
			if((ssize_t)(sizeof(u_int32_t)) == nwrit)
			{
				lc_info("%s: selection id successfully sent to peer %u using fd %d", __FUNCTION__, id, sockfd);
				set_peer_conn(id, sockfd);
				return;
			}
			else if(0 > nwrit)
			{
		        int errcode = errno;
		        char errmsg[256];
		        lc_error("%s: write() failed with error %d : [%s].",
		        		__FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
			}
			else
				lc_info("%s: invalid size %d returned from send selection id to peer %u using fd %d",
						__FUNCTION__, (int)nwrit, id, sockfd);
		}
		else
		{
	        int errcode = errno;
	        char errmsg[256];
	        lc_error("%s: connect() failed with error %d : [%s].",
	        		__FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
		}
		close(sockfd);
	}
	else
	{
        int errcode = errno;
        char errmsg[256];
        lc_error("%s: socket() failed with error %d : [%s].",
        		__FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
	}

	add_peer_connector(id, asec);
}

void comm_client_tcp_mesh::on_select_read(int conn_fd, short, void *)
{
	event_free(m_peers[m_id].reader);
	m_peers[m_id].reader = NULL;

	u_int32_t id = 0xFFFFFFFF;
	ssize_t nread = read(conn_fd, &id, sizeof(u_int32_t));
	if(sizeof(u_int32_t) == nread)
	{
		id = ntohl(id);
		if(m_peers.size() > id && id > m_id)
			set_peer_conn(id, conn_fd);
		else
		{
			lc_warn("%s: invalid id %u received on conn fd %d", __FUNCTION__, id, conn_fd);
			close(conn_fd);
		}
	}
	else if(0 == nread)
		lc_error("%s: selected conn %d was closed on peer side.", __FUNCTION__, conn_fd);
	else
	{
        int errcode = errno;
        char errmsg[256];
        lc_error("%s: read() failed with error %d : [%s].",
        		__FUNCTION__, errcode, strerror_r(errcode, errmsg, 256));
	}
    set_accept();
}

void comm_client_tcp_mesh::on_select_timeout(int conn_fd, short, void *)
{
	lc_warn("%s: timeout waiting for selection id on conn fd %d.", __FUNCTION__, conn_fd);
	close(conn_fd);
    set_accept();
}

int comm_client_tcp_mesh::set_peer_conn(const unsigned int id, int conn_fd)
{
	if(0 == pipe(m_peers[id].out_pipe))
	{
		m_peers[id].writer = event_new(the_base, m_peers[id].out_pipe[PIPE_READ_FD], EV_READ, comm_client_tcp_mesh::write1_cb, (void *)(m_peers.data() + id));
		if(0 == event_add(m_peers[id].writer, NULL))
		{
			m_peers[id].reader = event_new(the_base, conn_fd, EV_READ|EV_PERSIST, comm_client_tcp_mesh::read_cb, (void *)(m_peers.data() + id));
			if(0 == event_add(m_peers[id].reader, NULL))
			{
				m_peers[id].sockfd = conn_fd;
				this->m_sink->on_comm_up_with_party(id);
				lc_debug("%s: peer %u conn fd %d is set.", __FUNCTION__, id, conn_fd);
				return 0;
			}
			else
				lc_error("%s: failed adding the reader event of peer id %u with fd %d.", __FUNCTION__, id, conn_fd);
			event_free(m_peers[id].reader);
			m_peers[id].reader = NULL;

			event_del(m_peers[id].writer);
		}
		else
			lc_error("%s: failed adding the writer event of peer id %u with fd %d.", __FUNCTION__, id, conn_fd);
		event_free(m_peers[id].writer);
		m_peers[id].writer = NULL;

		close(m_peers[id].out_pipe[PIPE_READ_FD]);
		close(m_peers[id].out_pipe[PIPE_WRITE_FD]);
	}
	else
	{
		int errcode = errno;
		char errmsg[512];
		lc_error("%s: pipe() failed with error %d : %s", __FUNCTION__, errcode, strerror_r(errcode, errmsg, 512));
	}
	return -1;
}

void comm_client_tcp_mesh::disconnect_peer(const unsigned int id)
{
	lc_info("%s: peer %u is being disconnected.", __FUNCTION__, id);
	this->m_sink->on_comm_down_with_party(id);
	if(-1 != m_peers[id].sockfd) { close(m_peers[id].sockfd); m_peers[id].sockfd = -1; }
	if(-1 != m_peers[id].out_pipe[PIPE_READ_FD]) { close(m_peers[id].out_pipe[PIPE_READ_FD]); m_peers[id].out_pipe[PIPE_READ_FD] = -1; }
	if(-1 != m_peers[id].out_pipe[PIPE_WRITE_FD]) { close(m_peers[id].out_pipe[PIPE_WRITE_FD]); m_peers[id].out_pipe[PIPE_WRITE_FD] = -1; }
	if(NULL != m_peers[id].reader) { event_del(m_peers[id].reader); event_free(m_peers[id].reader); m_peers[id].reader = NULL; }
	if(NULL != m_peers[id].writer) { event_del(m_peers[id].writer); event_free(m_peers[id].writer); m_peers[id].writer = NULL; }
	if(id < m_id)
		add_peer_connector(id, zeroto);
}

void comm_client_tcp_mesh::on_write1(int fd, short what, void * arg)
{
	peer_t * peer = (peer_t *)arg;
	event_free(peer->writer);
	peer->writer = NULL;

	peer->writer = event_new(the_base, peer->sockfd, EV_WRITE, comm_client_tcp_mesh::write2_cb, (void *)(m_peers.data() + peer->id));
	if(0 != event_add(peer->writer, NULL))
	{
		lc_error("%s: failed adding the writer event of peer id %u with fd %d.", __FUNCTION__, peer->id, peer->sockfd);
		event_free(peer->writer);
		peer->writer = NULL;
		disconnect_peer(peer->id);
	}
	else
		lc_debug("%s: added a writer event of peer id %u with fd %d.", __FUNCTION__, peer->id, peer->sockfd);
}

void comm_client_tcp_mesh::on_write2(int fd, short what, void * arg)
{
	peer_t * peer = (peer_t *)arg;
	event_free(peer->writer);
	peer->writer = NULL;

	int result = splice(peer->out_pipe[PIPE_READ_FD], NULL, peer->sockfd, NULL, 4096, 0);
	if(0 > result)
	{
		int errcode = errno;
		char errmsg[512];
		lc_error("%s: splice() failed with error %d : %s", __FUNCTION__, errcode, strerror_r(errcode, errmsg, 512));
	}
	else
		lc_debug("%s: %d bytes spliced out to peer id %u with fd %d.", __FUNCTION__, result, peer->id, peer->sockfd);

	peer->writer = event_new(the_base, peer->out_pipe[PIPE_READ_FD], EV_READ, comm_client_tcp_mesh::write1_cb, (void *)(m_peers.data() + peer->id));
	if(0 != event_add(peer->writer, NULL))
	{
		lc_error("%s: failed adding the writer event of peer id %u with fd %d.", __FUNCTION__, peer->id, peer->sockfd);
		event_free(peer->writer);
		peer->writer = NULL;
		disconnect_peer(peer->id);
	}
	else
		lc_debug("%s: added a writer event of peer id %u with fd %d.", __FUNCTION__, peer->id, peer->sockfd);
}

void comm_client_tcp_mesh::on_read(int fd, short what, void * arg)
{
	peer_t * peer = (peer_t *)arg;
	u_int8_t buffer[4096];
	ssize_t nread = read(peer->sockfd, buffer, 4096);
	if(0 > nread)
	{
		int errcode = errno;
		char errmsg[512];
		lc_error("%s: read() failed with error %d : %s", __FUNCTION__, errcode, strerror_r(errcode, errmsg, 512));
		disconnect_peer(peer->id);
	}
	else if(0 == nread)
	{
		lc_warn("%s: peer disconnected.", __FUNCTION__);
		disconnect_peer(peer->id);
	}
	else
	{
		lc_debug("%s: %d bytes read from peer id %u with fd %d.", __FUNCTION__, (int)nread, peer->id, peer->sockfd);
		this->m_sink->on_comm_message(peer->id, buffer, nread);
	}
}

void comm_client_tcp_mesh::timer_cb(evutil_socket_t fd, short what, void * arg)
{
	if(EV_TIMEOUT & what)
		((comm_client_tcp_mesh *)arg)->on_timer(fd, what, arg);
}

void comm_client_tcp_mesh::connect_cb(evutil_socket_t fd, short what, void * arg)
{
	if(EV_TIMEOUT & what)
		((comm_client_tcp_mesh *)arg)->on_connect(fd, what, arg);
}

void comm_client_tcp_mesh::accept_cb(evutil_socket_t fd, short what, void * arg)
{
	if(EV_READ & what)
		((comm_client_tcp_mesh *)arg)->on_accept(fd, what, arg);
}

void comm_client_tcp_mesh::select_cb(evutil_socket_t fd, short what, void * arg)
{
	if(0 != (EV_READ & what))
		((comm_client_tcp_mesh *)arg)->on_select_read(fd, what, arg);
	else if(0 != (EV_TIMEOUT & what))
		((comm_client_tcp_mesh *)arg)->on_select_timeout(fd, what, arg);
}

void comm_client_tcp_mesh::write1_cb(evutil_socket_t fd, short what, void * arg)
{
	if(EV_READ & what)
	{
		peer_t * peer = (peer_t *)arg;
		peer->client->on_write1(fd, what, arg);
	}
}

void comm_client_tcp_mesh::write2_cb(evutil_socket_t fd, short what, void * arg)
{
	if(EV_WRITE & what)
	{
		peer_t * peer = (peer_t *)arg;
		peer->client->on_write2(fd, what, arg);
	}
}

void comm_client_tcp_mesh::read_cb(evutil_socket_t fd, short what, void * arg)
{
	if(EV_READ & what)
	{
		peer_t * peer = (peer_t *)arg;
		peer->client->on_read(fd, what, arg);
	}
}
