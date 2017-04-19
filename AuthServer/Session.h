#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <boost/asio.hpp>
#include "NetWork/Socket.h"

/*
 * AU服务器的会话，包括对客户端和游戏服务器个模块的处理，由于处理的协议有限，此处集成在一起，方便维护.
 *
 * */

namespace Adoter
{

using namespace google::protobuf;

class Session : public Socket<Session>
{
public:
	typedef std::function<int32_t(Message*)> CallBack;
public:
	virtual ~Session() { }
	Session(boost::asio::ip::tcp::socket&& socket);
	Session(Session const& right) = delete;    
	Session& operator=(Session const& right) = delete;
	
	virtual void Start() override;
	virtual bool Update() override { return true; }
	//协议处理
	virtual void ProtocolHandler(const boost::system::error_code error, const std::size_t bytes_transferred);
};

class SessionManager : public SocketManager<Session> 
{
	typedef SocketManager<Session> SuperSocketManager;
public:
	static SessionManager& Instance()
	{
		static SessionManager _instance;
		return _instance;
	}

	bool StartNetwork(boost::asio::io_service& io_service, const std::string& bind_ip, int32_t port, int thread_count = 1) override;
	NetworkThread<Session>* CreateThreads() const override;
private:        
	static void OnSocketAccept(tcp::socket&& socket, int32_t thread_index);
};

#define SessionInstance SessionManager::Instance()

}
