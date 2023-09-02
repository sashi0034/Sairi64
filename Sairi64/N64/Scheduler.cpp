#include "stdafx.h"
#include "Scheduler.h"

namespace N64
{
	void Scheduler::Step(CpuCycles cycles)
	{
		if (m_eventQueue.empty()) return;

		m_currentTime += cycles;
		if (m_currentTime >= m_overflowTime)
		[[unlikely]]
		{
			// 時間オーバーフローしたときの対処
			handleOverflowTime();
		}

		while (true)
		{
			// キューは昇順ソートされているので、先頭要素だけを比較する
			auto&& top = m_eventQueue.top();
			if (m_currentTime >= top.GetFireTime())
			{
				// 発火
				top.Fire();
				m_eventQueue.pop();
				if (m_eventQueue.empty())
				{
					// キューがなくなったら、計測を終了
					m_currentTime = 0;
					return;
				}
			}
			else
			{
				return;
			}
		}
	}

	void Scheduler::handleOverflowTime()
	{
		// 全てoverflow分減らす
		m_currentTime -= m_overflowTime;

		// キューを作り直す
		priority_queue temp{};
		while (m_eventQueue.empty() == false)
		{
			auto next = std::move(m_eventQueue.top());
			next.SetFireTime(next.GetFireTime() - m_overflowTime);
			temp.push(next);
			m_eventQueue.pop();
		}
		m_eventQueue.swap(temp);
	}
}
