// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

namespace jngl {

/// Scale-independent pixels, see jngl::getScaleFactor()
class ScaleablePixels {
public:
	explicit ScaleablePixels(double);

private:
	double value;
};

} // namespace jngl

jngl::ScaleablePixels operator"" _sp(long double);
