#include "stdafx.h"
#include "N64Singleton.h"

namespace N64
{
	N64Singleton* s_instance = nullptr;

	N64Singleton& N64Singleton::Instance()
	{
		return *s_instance;
	}

	N64Singleton::N64Singleton()
	{
		s_instance = this;
	}

	N64Singleton::~N64Singleton()
	{
		s_instance = nullptr;
	}
}
