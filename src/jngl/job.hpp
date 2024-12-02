// Copyright 2012-2024 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
/// Contains jngl::Job class and related functions
/// @file
#pragma once

#include <filesystem>
#include <functional>
#include <memory>

namespace jngl {

/// Background job which stays part of the main loop independent of the active jngl::Work
///
/// This can be used for an achievement system for example. Override this class and use
/// jngl::addJob to register it.
///
/// Jobs (meaning instances of this class that were added using jngl::addJob) are always stepped
/// before the active jngl::Work and drawn after it.
class Job {
public:
	/// Advance the game logic
	///
	/// This function gets called 60 times per second or whatever has been set by
	/// setStepsPerSecond().
	///
	/// \note JNGL's main loop calls all Jobs' step() functions before the active Work's step()
	virtual void step() = 0;

	/// Draw the game state
	///
	/// As it isn't garuanteed that this function is called as often as Job::step, you shouldn't
	/// change any game state in it.
	///
	/// \note JNGL's main loop calls all Jobs' draw() functions after the active Work's draw()
	virtual void draw() const = 0;

	/// Called when a file has been dropped onto the window
	virtual void onFileDrop(const std::filesystem::path&);

	/// Does nothing
	Job() = default;

	/// Copy constructor
	Job(const Job&) = default;

	/// Copy assignment
	Job& operator=(const Job&) = default;

	/// Move constructor
	Job(Job&&) = default;

	/// Move assignment
	Job& operator=(Job&&) = default;

	/// Does nothing
	virtual ~Job();
};

/// Add a new Job which will be always be stepped and drawn by App::mainLoop()
///
/// Example:
/// \code
/// class MyJob : public jngl::Job {
/// public:
///     MyJob();
///     // ...
/// };
///
/// jngl::AppParameters jnglInit() {
///     jngl::AppParameters params;
///     // ...
///     params.start = []() {
///         jngl::addJob(std::make_shared<MyJob>());
///         // ...
///     };
///     return params;
/// }
/// \endcode
void addJob(std::shared_ptr<Job> job);

/// The same as addJob(std::shared_ptr<Job>) but creates the Job for you
///
/// Example:
/// \code
/// class MyJob : public jngl::Job {
/// public:
///     MyJob(int x, float y, std::string foo);
///     // ...
/// };
///
/// jngl::AppParameters jnglInit() {
///     jngl::AppParameters params;
///     // ...
///     params.start = []() {
///         jngl::addJob<MyJob>(42, 3.14f, "Hello, World!");
///         // ...
///     };
///     return params;
/// }
/// \endcode
template <class T, class... Args>
void addJob(Args&&... args) {
	addJob(std::make_shared<T>(std::forward<Args>(args)...));
}

/// Removes the passed Job after all Jobs have been stepped
///
/// If the Job isn't found, nothing happens.
void removeJob(Job*);

/// Returns the first Job for which \a predicate returned true
std::shared_ptr<Job> getJob(const std::function<bool(Job&)>& predicate);

/// Returns the first Job that is a T
template <class T> std::shared_ptr<T> getJob() {
	return std::dynamic_pointer_cast<T>(
	    getJob([](Job& job) { return dynamic_cast<T*>(&job) != nullptr; }));
}

} // namespace jngl
