#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include <AsyncLogging.hpp>
#include <AsyncLogging_p.hpp>
#include <LogFile.hpp>
#include <FileName.hpp>


namespace oct_tk
{
	/// --------------------------------------------------------------------------------
	/// @brief: 删除目录
	/// --------------------------------------------------------------------------------
	int AsyncLoggingPrivate::removeLogFile(const std::string& folder, const int clear_days)
	{
		/// 如果目录不存在，则pass
		if (false == FileName::ins().dirIsExists(folder))
		{
			return 0;
		}

		auto getDateTimeFromFileName = [](const std::string & str)
		{
			std::string ret;
			// 查找第一个逗号的位置
			size_t pos = str.find_last_of('/');
			if (pos != std::string::npos)
			{
				/// 得到文件, 2023_06_04__18_01_01_000.log
				std::string file_tmp = str.substr(pos+1);

				/// 得到日志，去除 .log后缀
				pos = file_tmp.find_last_of('.');
				if (pos != std::string::npos)
				{
					return file_tmp.substr(0, pos);
				}

			}
		
			return str;
		};

		/// 获取今日的日期
		std::string today = FileName::ins().formatNow();

		/// 存在， 检查当前目录下的所有文件
		FileName::VectorString vec_file = FileName::ins().allFile(folder, std::string(".log"));

		for (const std::string& dst_file : vec_file)
		{
			/// 得到日期： 2022_01_20__12_30_00_000
			const std::string file_date = getDateTimeFromFileName(dst_file);
			/// 计算天数插值
			const int diff_day = diffDays(file_date, today);
			/// 删除文件
			if (clear_days <= diff_day)
			{
				FileName::ins().deleteFile(dst_file);
			}
		}

		return 0;
	}

	/// --------------------------------------------------------------------------------
	/// @brief: 
	/// --------------------------------------------------------------------------------
	int AsyncLoggingPrivate::diffDays(const std::string& str1, const std::string& str2)
	{
		std::tm tm1{};
		std::istringstream ss1(str1);
		ss1 >> std::get_time(&tm1, "%Y_%m_%d__%H_%M_%S_");
		auto tp1 = std::chrono::system_clock::from_time_t(std::mktime(&tm1));
		// 因为这里 mm 表示毫秒，所以精度到纳秒级别
		int64_t ms1;
		ss1 >> ms1;
		tp1 += std::chrono::duration<int64_t, std::milli>(ms1);

		std::tm tm2{};
		std::istringstream ss2(str2);
		ss2 >> std::get_time(&tm2, "%Y_%m_%d__%H_%M_%S_");
		auto tp2 = std::chrono::system_clock::from_time_t(std::mktime(&tm2));
		int64_t ms2;
		ss2 >> ms2;
		tp2 += std::chrono::duration<int64_t, std::milli>(ms2);

		auto duration = tp2 - tp1; // 计算两个时间点的持续时间
		auto days = std::chrono::duration_cast<std::chrono::hours>(duration).count() / 24;

		return std::abs(days);	}

	/// --------------------------------------------------------------------------------
	/// @brief: 
	/// --------------------------------------------------------------------------------
	AsyncLoggingPrivate::AsyncLoggingPrivate()
		: flushInterval_(3),
		running_(false),
		basename_(""),
		rollSize_(20 * 1024 * 1024),
		thread_(std::bind(&AsyncLoggingPrivate::threadFunc, this), "Logging"),
		latch_(1),
		mutex_(),
		//cond_(mutex_),
		currentBuffer_(new Buffer),
		nextBuffer_(new Buffer),
		buffers_()
	{
		currentBuffer_->bzero();
		nextBuffer_->bzero();
		buffers_.reserve(16);
	}

	/// --------------------------------------------------------------------------------
	/// @brief: 
	/// --------------------------------------------------------------------------------
	AsyncLoggingPrivate::~AsyncLoggingPrivate()
	{

	}

	/// --------------------------------------------------------------------------------
	/// @brief: 
	/// --------------------------------------------------------------------------------
	void AsyncLoggingPrivate::threadFunc()
	{
		latch_.countDown();
		LogFile output(log_init_.foler_, rollSize_, true);
		BufferPtr newBuffer1(new Buffer);
		BufferPtr newBuffer2(new Buffer);
		newBuffer1->bzero();
		newBuffer2->bzero();
		BufferVector buffersToWrite;
		buffersToWrite.reserve(16);

		int xxx = 0;
		while (running_)
		{
			++xxx;
			{
				std::unique_lock<std::mutex> lock(mutex_);
				if (buffers_.empty())  // unusual usage!
				//while (buffers_.empty())
				{
					//cond_.waitForSeconds(flushInterval_, lock);
					condi_.wait_for(lock, std::chrono::seconds(flushInterval_));
				}
				buffers_.push_back(std::move(currentBuffer_));
				currentBuffer_ = std::move(newBuffer1);
				buffersToWrite.swap(buffers_);
				if (!nextBuffer_)
				{
					nextBuffer_ = std::move(newBuffer2);
				}
			}

			if (buffersToWrite.size() > 25)
			{
				buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
			}

			for (const auto& buffer : buffersToWrite)
			{
				output.append(buffer->data(), buffer->length());
			}

			if (buffersToWrite.size() > 2)
			{
				buffersToWrite.resize(2);
			}

			if (!newBuffer1)
			{
				newBuffer1 = std::move(buffersToWrite.back());
				buffersToWrite.pop_back();
				newBuffer1->reset();
			}

			if (!newBuffer2)
			{
				newBuffer2 = std::move(buffersToWrite.back());
				buffersToWrite.pop_back();
				newBuffer2->reset();
			}

			buffersToWrite.clear();
			output.flush();
		}
		output.flush();
	}


	/// --------------------------------------------------------------------------------
	/// @brief: 
	/// --------------------------------------------------------------------------------
	AsyncLogging::AsyncLogging()
	{
		d_ptr_.reset(new(std::nothrow) AsyncLoggingPrivate);
	}

	/// --------------------------------------------------------------------------------
	/// @brief: 
	/// --------------------------------------------------------------------------------
	AsyncLogging::~AsyncLogging()
	{
		if (d_ptr_->running_)
		{
			shutdown();
		}
	}

	/// --------------------------------------------------------------------------------
	/// @brief: 
	/// --------------------------------------------------------------------------------
	int AsyncLogging::init(const LogInit& log_init_param)
	{
		d_ptr_->setRollSize(log_init_param);
		/// 删除日志文件
		d_ptr_->removeLogFile(log_init_param.foler_, log_init_param.clear_day_);

		return 0;
	}

	/// --------------------------------------------------------------------------------
	/// @brief: 
	/// --------------------------------------------------------------------------------
	int AsyncLogging::begin()
	{
		d_ptr_->running_ = true;
		d_ptr_->thread_.start();
		d_ptr_->latch_.wait();
		return 0;
	}

	/// --------------------------------------------------------------------------------
	/// @brief: 
	/// --------------------------------------------------------------------------------
	int AsyncLogging::append(const std::string& log_str)
	{
		return append(log_str.c_str(), log_str.length());
	}

	/// --------------------------------------------------------------------------------
	/// @brief: 
	/// --------------------------------------------------------------------------------
	int AsyncLogging::append(const char* pdata, const size_t len)
	{
		if ((nullptr == pdata) || (0 == len))
		{
			return 1;
		}

		std::unique_lock<std::mutex> lock(d_ptr_->mutex_);
		const size_t current_len = (const size_t)d_ptr_->currentBuffer_->avail();
		if (current_len  > len)
		{
			d_ptr_->currentBuffer_->append(pdata, len);
		}
		else
		{
			d_ptr_->currentBuffer_->setEndChar();

			d_ptr_->buffers_.push_back(std::move(d_ptr_->currentBuffer_));

			if (d_ptr_->nextBuffer_)
			{
				d_ptr_->currentBuffer_ = std::move(d_ptr_->nextBuffer_);
			}
			else
			{
				d_ptr_->currentBuffer_.reset(new(std::nothrow)  AsyncLoggingPrivate::Buffer); 
			}
			d_ptr_->currentBuffer_->append(pdata, len);
			d_ptr_->condi_.notify_one();
		}

		return 0;
	}

	/// --------------------------------------------------------------------------------
	/// @brief: 
	/// --------------------------------------------------------------------------------
	int AsyncLogging::shutdown()
	{
		d_ptr_->running_ = false;
		//d_ptr_->cond_.notifyOne();
		d_ptr_->condi_.notify_one();
		d_ptr_->thread_.shutdown();

		return 0;
	}

}