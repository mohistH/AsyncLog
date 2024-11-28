#pragma once
#include <functional>
#include <thread>
#include <atomic>
#include <memory>

#include <typedef.hpp>
#include <CountDownLatch.hpp>

class ThreadPlus : public NonCopyable
{
public:
	using ThreadFunc = std::function<void()>;
public:
	ThreadPlus(ThreadFunc func, const std::string& name);
	~ThreadPlus();

	void start();
	void shutdown();
	bool isStarted();

private:
	ThreadFunc			func_{nullptr};
	std::atomic_bool	started_{false};
	std::atomic_bool	joined_{false};
	std::thread::id		thread_id_{};
	std::string			name_{""};
	CountDownLatch		latch_;
	std::unique_ptr<std::thread>	thread_{};
};