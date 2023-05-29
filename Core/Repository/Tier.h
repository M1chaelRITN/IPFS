#pragma once

#include <string>
#include <chrono>
using namespace std::chrono;

class Tier
{
public:
	static long long CHANGE_TIER_SEC;

	Tier();
	Tier(const Tier & other) = default;
	Tier & operator=(const Tier & other) = default;
	Tier(Tier && other) noexcept
		: m_tierLevel(other.m_tierLevel)
		, m_tierTime(other.m_tierTime)
		, m_tierCompliance(other.m_tierCompliance) {}
	Tier & operator=(Tier && other) noexcept {
		m_tierLevel = other.m_tierLevel;
		m_tierTime = other.m_tierTime;
		m_tierCompliance = other.m_tierCompliance;
		return *this;
	}

	void Set(int level, bool tierCompilance = true, time_point<system_clock> tierTime = system_clock::now());

	[[nodiscard]]
	bool IsChangeTier();
	void CompleteTier() { m_tierCompliance = true; }

	[[nodiscard]]
	int TierLevel() const { return m_tierLevel; }
	[[nodiscard]]
	bool TierCompliance() const { return m_tierCompliance; }
	[[nodiscard]]
	time_point<system_clock> Time() const { return m_tierTime; }

private:
	int m_tierLevel = 0;
	time_point<system_clock> m_tierTime;
	bool m_tierCompliance = false;

};
