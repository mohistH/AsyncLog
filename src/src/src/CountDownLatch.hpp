#pragma once
#include <mutex>
#include <condition_variable>
#include <typedef.hpp>

class CountDownLatch : public NonCopyable
{
public:
	explicit CountDownLatch(int count);

	/// -------------------------------------------------------------------------------
	/// @brief:		wait
	///  @ret:		void
	///				
	/// -------------------------------------------------------------------------------
	void wait();

	void countDown();

	int getCount();

private:
	mutable std::mutex mutex_{};
	std::condition_variable condi_{};
	int count_{ 0 };
};