#pragma once

template<class T>
class Singleton
{
public:
	static T& Instance()
	{
		static T t;
		return t;
	}

	Singleton() = default;
	virtual ~Singleton() = default;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
};