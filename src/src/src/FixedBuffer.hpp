#pragma once
#include <typedef.hpp>

namespace oct_tk
{
	template<size_t SIZE>
	class FixedBuffer
	{
	public:
		FixedBuffer()
			: cur_(data_)
		{
			
		}

		~FixedBuffer()
		{
			
		}

		void append(const char* /*restrict*/ buf, size_t len)
		{
			// FIXME: append partially
			if (implicit_cast<size_t>(avail()) > len)
			{
				memcpy(cur_, buf, len);
				cur_ += len;
				current_len_ += len;
			}
		}

		const char* data() const 
		{
			return data_; 
		}
		
		int length() const 
		{ 
			return static_cast<int>(cur_ - data_); 
		}

		// write to data_ directly
		char* current() 
		{ 
			return cur_; 
		}
		
		int avail() const 
		{ 
			return static_cast<int>(end() - cur_); 
		}

		void add(size_t len) 
		{ 
			cur_ += len; 
			current_len_ += len;
		}

		void reset() 
		{ 
			cur_ = data_; 
			current_len_ = 0;
		}
		
		void bzero() 
		{ 
			memset(data_, 0, sizeof(SIZE));
			current_len_ = 0;
		}

		void setEndChar()
		{
			data_[current_len_] = '\0';
		}

		//// for used by unit test
		//std::string toString() const 
		//{ 
		//	return std::string(data_, length());
		//}

		size_t currentLen()
		{
			return current_len_;
		}

	private:
		const char* end() const 
		{ 
			return data_ + sizeof(data_); 
		}

	private:
		char data_[SIZE] = {0};
		char* cur_ = nullptr;
		size_t	current_len_ = 0;
	};
}