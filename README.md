# 0 源码获取
- [gitee](https://gitee.com/mohistH/async-log)


# 1 概述
- 这是一个基于`c++11`的异步日志库
- 目前仅支持windows
- 已通过 VS2019 + windows11 测试。 
- 支持自动清理距今指定天数的日志
- 支持设定日志文件输出路径

## 2 待实现
- 支持linux

## 3 一个范例
- 使用范例
```
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
	/// 要写入的日志
	const std::string lon_line{"1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ~@#$%^&*()_-+=[]{}.,?\n" };
	/// 写入
	async_log->append(lon_line.c_str(), len);
	/// 释放
	async_log->shutdown();
```