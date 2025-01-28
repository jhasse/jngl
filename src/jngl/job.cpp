// Copyright 2012-2025 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "job.hpp"

#include "../windowptr.hpp"

namespace jngl {

Job::~Job() = default;

void Job::onFileDrop(const std::filesystem::path&) {
}

void Job::onControllersChanged() {
}

void addJob(std::shared_ptr<Job> job) {
	pWindow->addJob(std::move(job));
}

void removeJob(Job* job) {
	pWindow->removeJob(job);
}

std::shared_ptr<Job> getJob(const std::function<bool(Job&)>& predicate) {
	return pWindow->getJob(predicate);
}

} // namespace jngl
