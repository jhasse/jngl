// Copyright 2023-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Achievement class
/// @file
#pragma once

#include <functional>
#include <string>

namespace jngl {

class Sprite;

/// Represents one unlockable Achievement (aka Trophy)
///
/// This struct holds all the necessary information about a specific achievement and provides integration to the Steam SDK (if available). One way to handle achievements is to create a Singleton class which holds all achievements as member variables:
///
/// \code{.cpp}
/// #include <jngl/Achievement.hpp>
/// #include <jngl/Singleton.hpp>
///
/// struct Achievements : public jngl::Singleton<Achievements> {
///     jngl::Achievement finishLevel5{"FINISH_LEVEL_5", "Finish Level 5",
///                                    "Finish level 5", "finish_level_5"};
///	    jngl::Achievement collect100Coins{"COLLECT_100_COINS", "Collect 100 Coins",
///	                                      "Collect 100 coins", "collect_100_coins",
///                                       0, 100};
/// };
/// \endcode
///
/// Then you can unlock the achievements like this:
///
/// \code{.cpp}
/// void onNextLevel() {
///     if (currentLevel >= 5 && Achievements::handle().finishLevel5.getValue() == 0) {
///         Achievements::handle().finishLevel5.increaseValue(1);
///     }
/// }
/// void onCollectCoin() {
///     Achievements::handle().collect100Coins.increaseValue(1);
/// }
/// \endcode
struct Achievement {
	/// The \a id must match the ID you set in Steamworks (if you use Steam, otherwise it's unused).
	Achievement(std::string id, std::string name, std::string description, std::string icon,
	            int initialValue = 0, int maxValue = 1);

	/// API Name
	std::string id;

	std::string name;
	std::string description;

	/// filename for achievement icon, will be loaded when the achievement is displayed
	std::string icon;

	int maxValue;

	int getValue() const;
	void increaseValue(int by);

	// if set "customDisplayValue(<value>) of customDisplayValue(<maxValue>)" will be displayed
	std::function<int(int)> customDisplayValue;

private:
	int value;
};

} // namespace jngl
