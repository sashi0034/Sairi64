#pragma once
#include "Forward.h"

namespace N64
{
	class ScheduledEvent
	{
	public:
		ScheduledEvent(uint64 fireTime, const std::function<void()>& event) :
			m_fireTime(fireTime),
			m_event(event)
		{
		}

		void SetFireTime(uint64 time) { m_fireTime = time; }
		uint64 GetFireTime() const { return m_fireTime; }
		void Fire() const { m_event(); }

		bool operator>(const ScheduledEvent& right) const
		{
			return this->GetFireTime() > right.GetFireTime();
		}

	private:
		uint64 m_fireTime{};
		std::function<void()> m_event{};
	};

	class Scheduler
	{
	public:
		explicit Scheduler() = default;
		explicit Scheduler(uint64 overflow) : m_overflowTime(overflow) { return; }

		void ScheduleEvent(uint64 cycles, const std::function<void()>& event)
		{
			// イベントは発火時間昇順で挿入される
			m_eventQueue.emplace(ScheduledEvent(m_currentTime + cycles + 1, event));
		}

		void Step(CpuCycles cycles);

	private:
		uint64 m_currentTime{};
		uint64 m_overflowTime{UINT64_MAX >> 1};

		using priority_queue = std::priority_queue<
			ScheduledEvent, std::vector<ScheduledEvent>, std::greater<ScheduledEvent>>;
		priority_queue m_eventQueue{};

		void handleOverflowTime();
	};
}
