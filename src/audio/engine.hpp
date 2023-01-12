#pragma once

#include "stream.hpp"
#include "track.hpp"
#include "channel.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace psemek::audio
{

	struct engine
	{
		engine();
		~engine();

		channel_ptr output();
		void setPause(bool);

	private:
		struct impl;
		std::unique_ptr<struct impl> pimpl_;
		struct impl & impl() { return *pimpl_; }
		struct impl const & impl() const { return *pimpl_; }
		template <typename ... Args> static auto make_impl(Args && ... args) { return std::make_unique<struct impl>(std::forward<Args>(args)...); }
	};

}
