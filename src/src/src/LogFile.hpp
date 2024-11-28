#pragma once
#include <string.h>
#include <memory>
#include <mutex>
#include <typedef.hpp>

namespace FileUtil
{
	class AppendFile;
}

/// ----------------------------------------------
/// @brief: 
/// ----------------------------------------------
class LogFile : public NonCopyable
{
public:
public:
	LogFile(const std::string& folder_name,
		off_t rollSize,
		bool threadSafe = true,
		int flushInterval = 3,
		int checkEveryN = 1024);

	~LogFile();

	void append(const char* logline, const size_t len);
	void flush();
	bool rollFile();


private:
	void append_unlocked2(const char* logline, int len);

private:

	const std::string folder_name_;
	const off_t rollSize_;
	const int flushInterval_;
	const int checkEveryN_;

	int count_;

	std::unique_ptr<std::mutex> mutex_;
	time_t startOfPeriod_;
	time_t lastRoll_ = 0;
	time_t lastFlush_;
	std::unique_ptr<FileUtil::AppendFile> file_;

	const static int kRollPerSeconds_ = 60 * 60 * 24;
};