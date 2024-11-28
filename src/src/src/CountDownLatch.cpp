#include <CountDownLatch.hpp>


/// --------------------------------------------------------------------------------
/// @brief: 
/// --------------------------------------------------------------------------------
CountDownLatch::CountDownLatch(int count)
	:mutex_()
	, count_(count)
{

}

/// --------------------------------------------------------------------------------
/// @brief: 
/// --------------------------------------------------------------------------------
void CountDownLatch::wait()
{
	std::unique_lock<std::mutex> lock(mutex_);
	while (count_ > 0)
	{
		condi_.wait(lock);
	}
}

/// --------------------------------------------------------------------------------
/// @brief: 
/// --------------------------------------------------------------------------------
void CountDownLatch::countDown()
{
	std::unique_lock<std::mutex> lock(mutex_);
	--count_;

	if (0 >= count_ )
	{
		condi_.notify_all();
	}
}

/// --------------------------------------------------------------------------------
/// @brief: 
/// --------------------------------------------------------------------------------
int CountDownLatch::getCount() 
{
	std::unique_lock<std::mutex> lock(mutex_);
	return count_;
}