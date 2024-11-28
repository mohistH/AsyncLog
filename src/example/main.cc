
#include <iostream>
#include <thread>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <AsyncLogging.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

/// 获取可执行程序路径
std::string getExecutablePath()
{
	char buffer[4096] = {0};
#ifdef _WIN32
	GetModuleFileName(NULL, buffer, sizeof(buffer));
#else
	auto len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	if (len != -1)
	{
		buffer[len] = 0;
	}
#endif
	std::string path(buffer);
	/// 统一使用‘/’
	std::replace(path.begin(), path.end(), '\\', '/');

	return path.substr(0, path.rfind('/'));
	
}

const int kNumberOfThreads = 20;
const int kNumberOfLogsPerThread = 10000 * 100;
const std::string lon_line{"1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ~@#$%^&*()_-+=[]{}.,?\n" };

void thread_worker(oct_tk::AsyncLogging* logger, const int num_logs)
{
	const int len = lon_line.length();
	for (int i = 0; i < num_logs; ++i)
	{
		logger->append(lon_line.c_str(), len);
	}
}

int main()
{
	std::unique_ptr<oct_tk::AsyncLogging>async_log = std::make_unique<oct_tk::AsyncLogging>();
	/// 设置日志记录目录
	oct_tk::LogInit li;
	li.rolling_size_ = 20;
	/// 将日志记录在可可秩序程序所在目录下的log文件夹
	li.foler_ = getExecutablePath() + std::string("/log");
	/// 1. 初始化， 参数，设置日志记录路径等参数。
	async_log->init(li);
	///  2. 启动日志落地线程
	async_log->begin();

	/// 3. 创建工作线程
	std::vector<std::thread> threads;
	for (int i = 0; i < kNumberOfThreads; ++i)
	{
		threads.emplace_back([&async_log]()
			{
			thread_worker(async_log.get(), kNumberOfLogsPerThread / kNumberOfThreads);
			});
	}
	// 创建多个线程往日志中写入数据
	/// 5. 计算起始时间
	auto start_time = std::chrono::high_resolution_clock::now();
	for (auto& th : threads)
	{
		th.join();
	}
	auto finish_time = std::chrono::high_resolution_clock::now();
	// 记录开始和结束的时间点，并等待所有线程执行完毕

	auto elapsed_time_us =
		std::chrono::duration_cast<std::chrono::microseconds>(finish_time - start_time).count();
	
	std::cout << "\n" << "Total logs written: " << kNumberOfLogsPerThread;
	std::cout << "\n" << "Elapsed time: " << elapsed_time_us / 1000000.0 << " seconds";
	std::cout << "\n" << "Average time per log: " << elapsed_time_us / static_cast<double>(kNumberOfLogsPerThread) << " microseconds";

	int64_t logs_per_second = static_cast<int64_t>(1000000.0 /
		(elapsed_time_us / static_cast<double>(kNumberOfLogsPerThread * kNumberOfThreads)));
	std::cout << "\n" << "Maximum logs per second: " << logs_per_second;

	async_log->shutdown();

    return 0;
}