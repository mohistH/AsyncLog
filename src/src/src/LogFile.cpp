#include <iostream>
#include <LogFile.hpp>
#include <AppendFile.hpp>
#include <FileName.hpp>


/// --------------------------------------------------------------------------------
/// @brief: 
/// --------------------------------------------------------------------------------
LogFile::LogFile(const std::string& basename, off_t rollSize, bool threadSafe /*= true*/, int flushInterval /*= 3*/, int checkEveryN /*= 1024*/)
	: folder_name_(basename),
	rollSize_(rollSize),
	flushInterval_(flushInterval),
	checkEveryN_(checkEveryN),
	count_(0),
	mutex_(threadSafe ? new(std::nothrow)  std::mutex : nullptr),
	startOfPeriod_(0),
	lastRoll_(0),
	lastFlush_(0)
{
	//rollFile();
	std::string filename = FileName::ins().name(folder_name_);
	file_.reset(new FileUtil::AppendFile(filename));
	file_->setFile(filename);
}

/// --------------------------------------------------------------------------------
/// @brief: 
/// --------------------------------------------------------------------------------
LogFile::~LogFile()
{

}

/// --------------------------------------------------------------------------------
/// @brief: 
/// --------------------------------------------------------------------------------
void LogFile::append(const char* logline, const size_t len)
{
	if (mutex_)
	{
		std::unique_lock<std::mutex> lock(*mutex_);
		append_unlocked2(logline, len);
	}
	else
	{
		append_unlocked2(logline, len);
	}
}

/// --------------------------------------------------------------------------------
/// @brief: 
/// --------------------------------------------------------------------------------
void LogFile::flush()
{
	if (nullptr == file_)
	{
		return;
	}

	if (mutex_)
	{
		std::unique_lock<std::mutex> lock(*mutex_);
		file_->flush();
	}
	else
	{
		file_->flush();
	}
}

/// --------------------------------------------------------------------------------
/// @brief: 
/// --------------------------------------------------------------------------------
bool LogFile::rollFile()
{
	time_t now = 0;
	std::string filename = FileName::ins().name(folder_name_);
	if (0 == filename.length())
	{
		return false;
	}

#ifdef use_old
	time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

	if (now > lastRoll_)
	{
		lastRoll_ = now;
		lastFlush_ = now;
		startOfPeriod_ = start;
		file_.reset(new FileUtil::AppendFile(filename));
		return true;
	}
#else
	file_.reset(new FileUtil::AppendFile(filename));
	if (nullptr == file_)
	{
		return false;
	}
	return true;
#endif ///!
	return false;
}

/// --------------------------------------------------------------------------------
/// @brief: 
/// --------------------------------------------------------------------------------
void LogFile::append_unlocked2(const char* logline, int len)
{
	if (nullptr == file_)
	{
		return;
	}

	/// 检查当前文件还剩余多少空间。如果不足存放当前写入的内容，则新创建的一个文件
	const off_t rest_len = rollSize_ - file_->writtenBytes();

	/// 不足，创建新的文件
	if ((off_t)len > rest_len)
	{
		std::cout << "\n" << "len=" << len << ", file_len=" << file_->writtenBytes() << ", rest=" << rest_len;
		file_->flush();
		rollFile();
		count_ = 0;
		file_->append(logline, len);
		return;
	}

	/// 继续存日志
	file_->append(logline, len);

	++count_;
	if (count_ >= checkEveryN_)
	{
		count_ = 0;
		file_->flush();
	}

}
