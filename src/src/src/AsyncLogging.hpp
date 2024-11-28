#pragma once

#include <string>
#include <memory>

namespace oct_tk
{
	/// 定义导出符
/// ---------------------------------------------------------------------------------------
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32)
	/// ---------------------------------------------------------------------------------------------
#ifndef LIB_ASYNC_LOG
#define LIB_ASYNC_LOG	__declspec(dllexport)
#else
#define LIB_ASYNC_LOG	__declspec(dllimport)
#endif /// !LIB_ASYNC_LOG

#elif defined(_unix_) || defined(_linux_) || defined(_unix) || defined(_linux) || defined(__unix)
	/// ---------------------------------------------------------------------------------------------
#ifndef LIB_ASYNC_LOG
#define LIB_ASYNC_LOG	__attribute__((visibility ("default")))
#endif /// !LIB_ASYNC_LOG

#endif /// !defined(_WIN32) || defined(_WIN64) || defined(WIN32)




	/// ----------------------------------------------
	/// @brief: 日志模块初始化参数
	/// ----------------------------------------------
	struct LOG_INIT
	{
		/// 日志存放于哪个文件夹, 比如： C:/A
		/// 同时，清除日志文件也是这里
		std::string		foler_{};
		/// 滚动日志大小, 单位，MB, 默认大小为10MB
		size_t			rolling_size_ = 10;
		/// 自动清理距今少多天的日志文件， 默认删除 距今五天前的日志文件
		size_t			clear_day_{ 5 };
	};
	using LogInit = LOG_INIT;


	class AsyncLoggingPrivate;

	/// ----------------------------------------------
	/// @brief: 异步日志库, 日志文件格式： 2023_05_28__17_02_678.log
	/// ----------------------------------------------
	class LIB_ASYNC_LOG AsyncLogging
	{
	public:
		AsyncLogging();
		virtual ~AsyncLogging();

		AsyncLogging(const AsyncLogging&) = delete;
		AsyncLogging(const AsyncLogging&&) = delete;
		AsyncLogging& operator= (const AsyncLogging&) = delete;
		AsyncLogging& operator = (const AsyncLogging&&) = delete;

		/// -------------------------------------------------------------------------------
		/// @brief:		初始化, 将删除参数中指定目录下的日志文件
		/// @param: 	const LogInit & log_init_param - 
		///  @ret:		int
		///				
		/// -------------------------------------------------------------------------------
		int init(const LogInit& log_init_param);

		/// -------------------------------------------------------------------------------
		/// @brief:		启动线程
		///  @ret:		int
		///				
		/// -------------------------------------------------------------------------------
		int begin();

		/// -------------------------------------------------------------------------------
		/// @brief:		追加日志内容
		/// @param: 	const std::string & log_str - 
		///  @ret:		int
		///				
		/// -------------------------------------------------------------------------------
		int append(const std::string& log_str);
		
		/// -------------------------------------------------------------------------------
		/// @brief:		追加日志内容
		/// @param: 	const char * pdata - 
		/// @param: 	const size_t len - 
		///  @ret:		int
		///				
		/// -------------------------------------------------------------------------------
		int append(const char* pdata, const size_t len);

		/// -------------------------------------------------------------------------------
		/// @brief:		释放，结束线程的调用
		///  @ret:		int
		///				
		/// -------------------------------------------------------------------------------
		int shutdown();

	private:
		std::unique_ptr<AsyncLoggingPrivate>	d_ptr_{};
	};

}