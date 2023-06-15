// Copyright 2022-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#include "SteamAchievements.hpp"

#include "../jngl/Achievement.hpp"
#include "../jngl/debug.hpp"

namespace jngl {

SteamAchievements::SteamAchievements()
: callbackUserStatsReceived(this, &SteamAchievements::onUserStatsReceived),
  callbackUserStatsStored(this, &SteamAchievements::onUserStatsStored),
  callbackAchievementStored(this, &SteamAchievements::onAchievementStored) {
	const auto utils = SteamUtils();
	if (!utils) {
		throw std::runtime_error("SteamUtils() returned nullptr");
	}
	appID = utils->GetAppID();
	const auto user = SteamUser();
	if (!user) {
		throw std::runtime_error("SteamUser() returned nullptr");
	}
	if (!user->BLoggedOn()) {
		debugLn("Steam user not logged on");
		return;
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
	AchievementLayer::step();
}

void SteamAchievements::notify(const Achievement& achievement, int oldValue, int newValue) {
	if (initialized && newValue >= achievement.maxValue) {
		SteamUserStats()->SetAchievement(achievement.id.c_str());
		if (!SteamUserStats()->StoreStats()) {
			debugLn("Error setting Steam achievement.");
		}
	}
	AchievementLayer::notify(achievement, oldValue, newValue);
}

void SteamAchievements::onUserStatsReceived(UserStatsReceived_t* callback) {
	if (callback->m_nGameID != appID) {
		return;
	}
	if (callback->m_eResult != k_EResultOK) {
		return;
	}
	initialized = true;
}

void SteamAchievements::onUserStatsStored(UserStatsStored_t*) {
}

void SteamAchievements::onAchievementStored(UserAchievementStored_t*) {
}

void initSteam(uint32_t appId) {
#ifdef NDEBUG
	if (SteamAPI_RestartAppIfNecessary(appId)) {
		std::exit(1);
	}
#endif
	if (!SteamAPI_Init()) {
		throw std::runtime_error("SteamAPI_Init() failed. steam_appid.txt missing?");
	}
}

void initSteamAchievements() {
	addJob<SteamAchievements>();
}

} // namespace jngl
