#pragma once

namespace N64::Joybus
{
	// https://n64brew.dev/wiki/Joybus_Protocol#0xFF_-_Reset_/_Info
	// https://github.com/SimoneN64/Kaizen/blob/74dccb6ac6a679acbf41b497151e08af6302b0e9/src/backend/core/mmio/PIF.hpp#L61
	enum class JoybusType : uint8
	{
		None,
		Controller,
		DancePad,
		VRU,
		Mouse,
		RandnetKeyboard,
		DenshaDeGo,
		Eeprom4KB,
		Eeprom16KB
	};

	enum class AccessorType
	{
		None,
		MemPack,
		RumblePack,
	};

	class JoybusDevice
	{
	public:
		JoybusDevice() = default;
		virtual ~JoybusDevice() = default;

		virtual JoybusType Type() const = 0;
		virtual AccessorType Accessor() const = 0;
		bool HasAccessor() const { return Accessor() != AccessorType::None; }
	};

	class EmptyDevice : public JoybusDevice
	{
	public:
		JoybusType Type() const override { return JoybusType::None; }
		AccessorType Accessor() const override { return AccessorType::None; }
	};

	constexpr uint8 JoybusCount_6 = 6;

	class JoybusDeviceManager
	{
	public:
		JoybusDeviceManager();
		template <class T = JoybusDevice> T* TryGet(uint8 channel);

	private:
		std::array<std::unique_ptr<JoybusDevice>, JoybusCount_6> m_devices{};
	};

	template <class T>
	T* JoybusDeviceManager::TryGet(uint8 channel)
	{
		if (channel >= JoybusCount_6) return nullptr;
		if constexpr (std::same_as<T, JoybusDevice>) return m_devices[channel].get();
		return dynamic_cast<T*>(m_devices[channel].get());
	}
}
