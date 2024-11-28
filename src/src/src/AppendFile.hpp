#pragma once
#include <string>
#include <typedef.hpp>

namespace FileUtil
{
	class AppendFile : public NonCopyable
	{
	public:
		explicit AppendFile(const std::string filename);
		AppendFile() {}

		~AppendFile();

		void setFile(const std::string filename);

		void append(const char* logline, size_t len);

		void flush();

		off_t writtenBytes() const 
		{ 
			return writtenBytes_; 
		}

	private:

		bool write(const char* logline, size_t len);

		FILE* fp_ = nullptr;
		char buffer_[64 * 1024] = {0};
		off_t writtenBytes_ = 0;
	};
}