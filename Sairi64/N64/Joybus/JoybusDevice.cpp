#include "stdafx.h"
#include "JoybusDevice.h"

#include "Controller.h"

namespace N64::Joybus
{
	JoybusDeviceManager::JoybusDeviceManager()
	{
		for (int i = 0; i < JoybusCount_6; ++i)
		{
			// TODO: このあたりをちゃんとする
			m_devices[i] = std::make_unique<Controller>(i);
		}
	}
}
