
#pragma once

#include <event2/util.h>

class comm_client_tcp_mesh : public comm_client
{
	typedef struct
	{
		size_t id;
		int sockfd;
		int out_pipe[2];
		std::string ip;
		u_int16_t port;
		struct sockaddr_in inet_addr;
		struct event * reader;
		struct event * writer;
		class comm_client_tcp_mesh * client;
	}peer_t;

	std::vector< peer_t > m_peers;
	struct event_base * the_base;
	void run();
	void set_syslog_name();
	int load_peers(const unsigned int peer_count);
	int insure_resource_limits();
	int start_service();
	int set_accept();
	int add_connectors();
	int add_peer_connector(const unsigned int id, const struct timeval & zeroto);
	void clear_peers();
	int set_peer_conn(const unsigned int id, int conn_fd);
	void disconnect_peer(const unsigned int id);

	void on_timer(int fd, short what, void * arg);
	void on_accept(int fd, short what, void * arg);
	void on_connect(int fd, short what, void * arg);
	void on_select_read(int fd, short what, void * arg);
	void on_select_timeout(int fd, short what, void * arg);
	void on_write1(int fd, short what, void * arg);
	void on_write2(int fd, short what, void * arg);
	void on_read(int fd, short what, void * arg);

public:
	comm_client_tcp_mesh(comm_client::cc_args_t * args);
	virtual ~comm_client_tcp_mesh();

	virtual int start(const unsigned int id, const unsigned int peer_count, const char * comm_conf_file, comm_client_cb_api * sink);
	virtual void stop();

	int send(const unsigned int dst_id, const unsigned char * msg, const size_t size);

	static void timer_cb(evutil_socket_t fd, short what, void * arg);
	static void connect_cb(evutil_socket_t fd, short what, void * arg);
	static void accept_cb(evutil_socket_t fd, short what, void * arg);
	static void select_cb(evutil_socket_t fd, short what, void * arg);
	static void write1_cb(evutil_socket_t fd, short what, void * arg);
	static void write2_cb(evutil_socket_t fd, short what, void * arg);
	static void read_cb(evutil_socket_t fd, short what, void * arg);
};
