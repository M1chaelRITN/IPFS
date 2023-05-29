#pragma once

#include <memory>

#include "Service/Service.h"

class NewInternetService final : public Service
{
public:
	NewInternetService();
	~NewInternetService() override = default;

	void Main() override;

private:
	Repository m_repository;
	std::shared_ptr<NetManager> m_netManager;
	std::unique_ptr<Security> m_security;

	void CheckTiers();

};
