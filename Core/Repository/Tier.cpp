#include "Tier.h"

long long Tier::CHANGE_TIER_SEC = 1 * 60 * 60; // 1 hour

Tier::Tier()
    : m_tierLevel(0)
	, m_tierTime(system_clock::now())
	, m_tierCompliance(false)
{
}

void Tier::Set(int level, bool tierCompliance /*= true*/, time_point<system_clock> tierTime /*= system_clock::now()*/)
{
	m_tierLevel = level;
	m_tierTime = tierTime;
	m_tierCompliance = tierCompliance;
}

bool Tier::IsChangeTier()
{
	if (m_tierLevel == 2) { return false; }

	time_point<system_clock> now = system_clock::now();
	duration<double> diff = now - m_tierTime;
	seconds sec = duration_cast<seconds>(diff);

	if (sec.count() >= CHANGE_TIER_SEC)
	{
		m_tierLevel++;
		m_tierTime = now;
		m_tierCompliance = false;
		return true;
	}

	return false;
}
