#pragma once

template<typename To, typename From>
inline To implicit_cast(From const& f)
{
	return f;
}


class NonCopyable
{
public:
	NonCopyable(const NonCopyable&) = delete;
	void operator=(const NonCopyable&) = delete;

protected:
	NonCopyable() = default;
	~NonCopyable() = default;
};