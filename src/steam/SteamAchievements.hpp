// Copyright 2022-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "../AchievementLayer.hpp"

#include <steam_api.h>

namespace jngl {

class SteamAchievements : public AchievementLayer {
public:
	SteamAchievements();
	~SteamAchievements() override;

private:
	void step() override;
	void notify(const Achievement&, int oldValue, int newValue) override;

	STEAM_CALLBACK(SteamAchievements, onUserStatsReceived, UserStatsReceived_t,
	               callbackUserStatsReceived);
	STEAM_CALLBACK(SteamAchievements, onUserStatsStored, UserStatsStored_t,
	               callbackUserStatsStored);
	STEAM_CALLBACK(SteamAchievements, onAchievementStored, UserAchievementStored_t,
	               callbackAchievementStored);

	uint32_t appID;
	bool initialized = false;
};

} // namespace jngl
