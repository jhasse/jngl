// Copyright 2022-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "SteamAchievements.hpp"

namespace jngl {

SteamAchievements::SteamAchievements()
: callbackUserStatsReceived(this, &SteamAchievements::onUserStatsReceived),
  callbackUserStatsStored(this, &SteamAchievements::onUserStatsStored),
  callbackAchievementStored(this, &SteamAchievements::onAchievementStored) {
	if (!SteamAPI_Init()) {
		throw std::runtime_error("SteamAPI_Init() failed");
	}
	appID = SteamUtils()->GetAppID();
	const auto user = SteamUser();
	if (!user) {
		throw std::runtime_error("SteamUser() returned nullptr");
	}
	if (!user->BLoggedOn()) {
		throw std::runtime_error("Steam user not logged on");
	}
	const auto userStats = SteamUserStats();
	if (!userStats) {
		throw std::runtime_error("SteamUserStats() returned nullptr");
	}
	userStats->RequestCurrentStats();
}

SteamAchievements::~SteamAchievements() {
	SteamAPI_Shutdown();
}

void SteamAchievements::step() {
	SteamAPI_RunCallbacks();
}

void initSteam() {
	addJob<SteamAchievements>();
}

} // namespace jngl
