#pragma once

#include <string>
#include <sstream>
#include <exception>


class ExceptionBase : public std::exception {
public:
	ExceptionBase(const std::string& message,
		const std::string& file,
		int line,
		const std::string& function = "") noexcept;

	virtual ~ExceptionBase() noexcept = default;

	const char* what() const noexcept override;
	virtual std::string GetDetailedInfo() const;
	const std::string& getMessage() const noexcept { return m_message; }
	const std::string& getFile() const noexcept { return m_file; }
	int getLine() const noexcept { return m_line; }
	const std::string& getFunction() const noexcept { return m_function; }

protected:
	virtual void formatWhat();

	std::string m_message;
	std::string m_file;
	int m_line;
	std::string m_function;
	std::string m_what;
};

// 访问空指针
class NullPointerException : public ExceptionBase {
public:
	using ExceptionBase::ExceptionBase;
};

// 命令行输入有误
class CommandException : public ExceptionBase {
public:
	using ExceptionBase::ExceptionBase;
};

// 函数参数有误
class InvalidArgumentException : public ExceptionBase {
public:
	using ExceptionBase::ExceptionBase;
};

// 输入输出异常
class IOException : public ExceptionBase {
public:
	using ExceptionBase::ExceptionBase;
};

// 数组访问越界
class OutOfRangeException : public ExceptionBase {
	using ExceptionBase::ExceptionBase;
};

// 死循环
class DeadLoopException : public ExceptionBase {
	using ExceptionBase::ExceptionBase;
};

// 网络异常
class NetworkException : public ExceptionBase {
	using ExceptionBase::ExceptionBase;
};

// 内存异常
class MemoryException : public ExceptionBase {
	using ExceptionBase::ExceptionBase;
};

// Jsong格式有误
class JsonFormatException : public ExceptionBase {
	using ExceptionBase::ExceptionBase;
};

// 其他
class RuntimeException : public ExceptionBase {
	using ExceptionBase::ExceptionBase;
};

template<typename ExceptionClass>
[[noreturn]] void ThrowException(const std::string& message,
	const char* file,
	int line,
	const char* function = "") {
	throw ExceptionClass(message, file, line, function);
}

#define THROW_EXCEPTION(ExceptionClass, message) \
	ThrowException<ExceptionClass>(message, __FILE__, __LINE__, __FUNCTION__)