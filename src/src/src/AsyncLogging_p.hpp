#pragma once
#include <vector>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <string.h>

#include <FixedBuffer.hpp>
#include <CountDownLatch.hpp>
#include <ThreadPlus.hpp>
#include <AsyncLogging.hpp>
//#include "Condition.h"

namespace oct_tk
{
	class AsyncLoggingPrivate
	{
	public:
		enum
		{
			/// 缓存大小, 单位， 字节
			LARGE_BUFFER_SIZE = 1024 * 1024 * 1,
		};

		AsyncLoggingPrivate();
		~AsyncLoggingPrivate();

	public:
		/// 线程函数
		void threadFunc();

		/// 比较 str1与 str2之间差了几天, str2的时间大于str1的时间
		int diffDays(const std::string& str1, const std::string& str2);
		/// 删除目录
		int removeLogFile(const std::string& folder, const int clear_days);

		void setRollSize(const oct_tk::LogInit& init_param)
		{
			log_init_ = init_param;
			rollSize_ = log_init_.rolling_size_ * 1024 * 1024;
		}

	public:
		using Buffer = FixedBuffer<LARGE_BUFFER_SIZE>;
		using BufferVector = std::vector<std::unique_ptr<Buffer>> ;
		using BufferPtr = BufferVector::value_type;
		
		/// 单位： 秒
		const int flushInterval_ = 3;
		std::atomic_bool running_{false};
		const std::string basename_{};
		off_t rollSize_{};
		ThreadPlus thread_;
		CountDownLatch latch_;
		std::mutex mutex_{};

		/// mutex_
		//Condition cond_;
		std::condition_variable condi_{};
		/// mutex_
		BufferPtr currentBuffer_{};
		/// mutex_
		BufferPtr nextBuffer_{};
		/// mutex_
		BufferVector buffers_{};

		/// 一些参数
		LogInit		log_init_{};
	};

}