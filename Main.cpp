/*
 * C++版本：C++11
 *
 * 说明：倾向于用标准类库中接口
 *
 */

#include <thread>
#include <vector>
#include <memory>
#include <functional>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/format.hpp>
#include <boost/progress.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "Timer.h"
#include "World.h"
#include "WorldSession.h"
#include "Log.h"
#include "MessageDispatcher.h"

using namespace boost;
using namespace boost::interprocess;

#define MAX_MSG_COUNT 50000
#define MAX_MSG_SIZE 1024

const int const_world_sleep = 50;

/*
 * 游戏逻辑处理接口
 *
 * */

using namespace Adoter;

boost::asio::io_service _io_service;
std::shared_ptr<boost::asio::io_service::work> _io_service_work;

void SignalHandler(const boost::system::error_code& error, int)
{    
	//if (!error) World::StopNow(SHUTDOWN_EXIT_CODE);
}

void WorldUpdateLoop()
{
	int32_t curr_time = 0, prev_sleep_time = 0;
	
	int32_t prev_time = GetTime();
	
	while (!WorldInstance.IsStopped())
	{
		curr_time = GetTime();
		
		int32_t diff = GetTimeDiff(prev_time, curr_time);
		
		WorldInstance.Update(diff);        
		
		prev_time = curr_time;
		
		if (diff <= const_world_sleep + prev_sleep_time) //50MS
		{            
			prev_sleep_time = const_world_sleep + prev_sleep_time - diff;            
			std::this_thread::sleep_for(std::chrono::milliseconds(prev_sleep_time));        
			//std::cout << __func__ << ":" << prev_sleep_time << std::endl;
		}        
		else    
		{	
			prev_sleep_time = 0;
		}
	}
}

void ShutdownThreadPool(std::vector<std::shared_ptr<std::thread>>& threads)
{    
	_io_service.stop();    
	
	for (auto& thread : threads)    
	{        
		thread->join();    
	}
}

/*
 *
 * 函数入口
 *
 * */
int main(int argc, const char* argv[])
{


	/*

	 message_queue::remove("message_queue");

	 message_queue mq(create_only               //only create
		,"message_queue"           //name
		,MAX_MSG_COUNT                       //max message number
		,MAX_MSG_SIZE               //max message size
	);

	{
		progress_timer pt;//记录时间，多方便！
		        
		for(int i = 0; i < 5000; ++i)//可灵活调整i的大小
		{    
			std::string msg = str(format("hello world %d") % i);
			//bool bRet = mq.send(msg.c_str(),msg.size(),0);  
		}
	} 

*/


		/*	
		Asset::Item_Potion* message = new Asset::Item_Potion();
		message->mutable_item_common_prop()->set_quality(5);
		Item* item = new Item(message);	
		bool re = item->CanUse();
		Item* item_potion = new Item_Potion(message);	
		*/
	std::cout << "Service starting..." << std::endl;
	LOG(ERROR, "OnAddressAdded(): add address failed: [%s]: %s", "50000", "我是log");
	try 
	{
		//世界初始化，涵盖所有....
		if (!WorldInstance.Load()) return 1;

		//网络初始化
		_io_service_work = std::make_shared<boost::asio::io_service::work>(_io_service);

		int _thread_nums = 5;
		std::vector<std::shared_ptr<std::thread>> _threads;	
		for (int i = 0; i < _thread_nums; ++i)
		{
			std::shared_ptr<std::thread> pthread(new std::thread(boost::bind(&boost::asio::io_service::run, &_io_service)));	//IO <-> Multi Threads
			_threads.push_back(pthread);
		}

		DispatcherInstance.StartWork(); //消息处理机制

		//boost::asio::signal_set signals(_io_service, SIGINT, SIGTERM);
		//signals.async_wait(SignalHandler);

		WorldSessionInstance.StartNetwork(_io_service, "0.0.0.0", 40000, 5); //网络

		//世界循环
		WorldUpdateLoop();
	
		std::cout << "Service stop..." << std::endl;

		ShutdownThreadPool(_threads);
	}
	catch (std::exception& e)
	{
		std::cerr << __func__ << ":Exception: " << e.what() << std::endl;
	}
	
	std::cout << "Service stoped." << std::endl;
	return 0;
}
