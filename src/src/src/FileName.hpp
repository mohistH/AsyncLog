#pragma once
#include <string>
#include <chrono>
#include <ctime>
#include <vector>
#include <algorithm>
#include <typedef.hpp>

#ifdef _WIN32
/// windows
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>
#else
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/statfs.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/io.h>
#include <dirent.h>

#endif ///!


class FileName : public NonCopyable
{
public:
	using VectorString = std::vector<std::string>;

public:
	FileName() {}
	~FileName() {}

	static FileName& ins()
	{
		static FileName fn;
		return fn;
	}
	
	/// 参数为日志文件所在文件夹
	std::string name(const std::string& folder)
	{
		/// 如果创建失败
		if (false == makeFolder(std::ref(folder)))
		{
			return std::string{};
		}

		/// 文件夹创建成功， 构造日志文件名称
		return folder + std::string("/") + formatNow() + std::string{".log"};
	}

	/// 检查参数中目录是否存在
	bool dirIsExists(const std::string& folder)
	{
		struct stat st;

		return (0 == stat(folder.c_str(), &st) ? true : false);
	}

	/// 获取参数目录中指定后缀的文件
	VectorString allFile( std::string parse_path, std::string suffix)
	{
		int replace_pos = parse_path.find('\\');
		if (0 <= replace_pos)
		{
			std::replace(parse_path.begin(), parse_path.end(), '\\', '/');
		}

#ifdef _WIN32	
		intptr_t handle;
		struct _finddata_t file_info;

		// 构造搜索模式
		char search_pattern[256 * 4]{0};
		snprintf(search_pattern, sizeof(search_pattern) - 1, "%s/*%s", parse_path.c_str(), suffix.c_str());

		// 开始第一次查找
		handle = _findfirst(search_pattern, &file_info);
		if (handle == -1)
		{
			return VectorString{};
		}

		VectorString ret;
		do
		{
			if ((file_info.attrib & _A_SUBDIR) == 0)
			{
				ret.emplace_back(parse_path + std::string("/") + std::string(file_info.name));
			}
		} while (_findnext(handle, &file_info) == 0);

		_findclose(handle);
#else 
		DIR *dp = opendir(parse_path.c_str());
		if (!dp) 
		{
			return VectorString{};
		}

		VectorString ret;
		struct dirent *ep;
		while ((ep = readdir(dp))) 
		{
			if (ep->d_type == DT_REG) 
			{ 
				// 判断是否为普通文件
				std::string name = ep->d_name;
				if (name.size() > suffix.size() && name.substr(name.size() - suffix.size()) == suffix) 
				{ 
					// 判断文件后缀是否匹配
					std::string path = parse_path + "/" + name;
					ret.emplace_back(path);
				}
			}
		}

		closedir(dp);
#endif ///

		return ret;
	}

	/// 删除文件
	void deleteFile(const std::string& file)
	{
		remove(file.c_str());
	}

private:
	/// 创建文件夹
	bool makeFolder(const std::string& folder)
	{
		std::string parse_path = folder;
		{
			int replace_pos = parse_path.find('\\');
			if (0 <= replace_pos)
			{
				std::replace(parse_path.begin(), parse_path.end(), '\\', '/');
			}
		}

		{
			std::string str_tmp;

			///  创建目录
			std::vector<std::string> vec_path = strsplit(parse_path, std::string("/"));
			for (int index = 0; index < vec_path.size(); ++index)
			{
				str_tmp += vec_path[index] + std::string("/");
				if (false == makeSingleDirectory(str_tmp))
				{
					return false;
				}
			}
		}

		return true;
	}

	/// 创建单级目录
	bool makeSingleDirectory(const std::string& str)
	{
		if (true == dirIsExists(str))
		{
			return true;
		}

#ifdef _WIN32
		/// 不存在，则创建
		const int ret = _mkdir(str.c_str());
		if (0 != ret)
		{
			/// 如果已经存在，则也返回成功
			if (EEXIST == ret)
			{
				return true;
			}

			return false;
		}
		return true;
#else 
		const int ret = mkdir(str.c_str(), 0777);
		
		return (0 == ret);
#endif ///
	}

	std::vector<std::string> strsplit(std::string src_str, std::string split_ch)
	{
		int nPos = 0;
		std::vector<std::string> vec;

		nPos = src_str.find(split_ch.c_str());
		while (-1 != nPos)
		{
			std::string temp = src_str.substr(0, nPos);
			vec.push_back(temp);
			src_str = src_str.substr(nPos + 1);
			nPos = src_str.find(split_ch.c_str());
		}
		vec.push_back(src_str);
		return vec;
	}

public:
	/// 返回文件名： 2023_05_30__20_13_14_999
	std::string formatNow()
	{
#ifdef use_msecond
		auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

		auto current_time = std::chrono::duration_cast<std::chrono::milliseconds>(now_ms.time_since_epoch()).count();
		auto current_time_s = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::milliseconds(current_time)).count();
		std::size_t milliseconds = current_time % 1000;

		// 输出当前年月日时分秒毫秒
		std::time_t current_time_t = static_cast<std::time_t>(current_time_s);
		std::tm* local_tm = std::localtime(&current_time_t);
		char time_buf[80];
		std::strftime(time_buf, sizeof(time_buf), "%Y_%m_%d__%H_%M_%S", local_tm);
		//std::cout << "Current date and time: " << time_buf << "." << milliseconds << " ms" << std::endl;
		return std::string(time_buf) + std::string("_") + std::to_string(milliseconds);
#else 
		auto now = std::chrono::system_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count() % 1000;

		std::time_t time = std::chrono::system_clock::to_time_t(now);
		std::tm tm = *std::localtime(&time);

		char buffer[64]{0};
		std::strftime(buffer, 30, "%Y_%m_%d__%H_%M_%S", &tm);

		std::string result(buffer);
		result += "_" + std::to_string(ms % 1000) + "_" + std::to_string(us);
		return result;
#endif ///
	}
};