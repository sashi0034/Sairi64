#include "stdafx.h"
#include "JoybusDevice.h"

#include "Controller.h"

namespace N64::Joybus
{
	JoybusDeviceManager::JoybusDeviceManager()
	{
		// TODO: このあたりをちゃんとする
		m_devices[0] = std::make_unique<Controller>(0);
		m_devices[1] = std::make_unique<Controller>(1);
		m_devices[2] = std::make_unique<Controller>(2);
		m_devices[3] = std::make_unique<Controller>(3);
		m_devices[4] = std::make_unique<EmptyDevice>();
		m_devices[5] = std::make_unique<EmptyDevice>();
	}
}
