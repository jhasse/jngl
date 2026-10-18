// Copyright 2007-2020 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include <string>

namespace jngl {

std::string getSystemConfigPath();

#ifdef _WIN32
/// The user's Documents folder, wherever it's been moved to, e.g. by OneDrive
std::string getSystemDocumentsPath();
#endif

} // namespace jngl
