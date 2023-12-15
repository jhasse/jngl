# CMake invokes the toolchain file twice during the first build, but only once during subsequent rebuilds.
if(DEFINED ENV{_IOS_TOOLCHAIN_HAS_RUN})
  return()
endif()
set(ENV{_IOS_TOOLCHAIN_HAS_RUN} true)

# Cache what generator is used
set(USED_CMAKE_GENERATOR "${CMAKE_GENERATOR}")

# Check if using a CMake version capable of building combined FAT builds (simulator and target slices combined in one static lib)
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.14")
  set(MODERN_CMAKE YES)
endif()

# Get the Xcode version being used.
# Problem: CMake runs toolchain files multiple times, but can't read cache variables on some runs.
# Workaround: On the first run (in which cache variables are always accessible), set an intermediary environment variable.
#
# NOTE: This pattern is used in many places in this toolchain to speed up checks of all sorts
if(DEFINED XCODE_VERSION_INT)
  # Environment variables are always preserved.
  set(ENV{_XCODE_VERSION_INT} "${XCODE_VERSION_INT}")
elseif(DEFINED ENV{_XCODE_VERSION_INT})
  set(XCODE_VERSION_INT "$ENV{_XCODE_VERSION_INT}")
elseif(NOT DEFINED XCODE_VERSION_INT)
  find_program(XCODEBUILD_EXECUTABLE xcodebuild)
  if(NOT XCODEBUILD_EXECUTABLE)
    message(FATAL_ERROR "xcodebuild not found. Please install either the standalone commandline tools or Xcode.")
  endif()
  execute_process(COMMAND ${XCODEBUILD_EXECUTABLE} -version
          OUTPUT_VARIABLE XCODE_VERSION_INT
          ERROR_QUIET
          OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REGEX MATCH "Xcode [0-9\\.]+" XCODE_VERSION_INT "${XCODE_VERSION_INT}")
  string(REGEX REPLACE "Xcode ([0-9\\.]+)" "\\1" XCODE_VERSION_INT "${XCODE_VERSION_INT}")
  set(XCODE_VERSION_INT "${XCODE_VERSION_INT}" CACHE INTERNAL "")
endif()

# Assuming that xcode 12.0 is installed you most probably have ios sdk 14.0 or later installed (tested on Big Sur)
# if you don't set a deployment target it will be set the way you only get 64-bit builds
if(NOT DEFINED DEPLOYMENT_TARGET AND XCODE_VERSION_INT VERSION_GREATER 12.0)
  # Temporarily fix the arm64 issues in CMake install-combined by excluding arm64 for simulator builds (needed for Apple Silicon...)
  set(CMAKE_XCODE_ATTRIBUTE_EXCLUDED_ARCHS[sdk=iphonesimulator*] "arm64")
endif()

# Touch the toolchain variable to suppress the "unused variable" warning.
# This happens if CMake is invoked with the same command line the second time.
if(CMAKE_TOOLCHAIN_FILE)
endif()

# Fix for PThread library not in path
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)

# Specify named language support defaults.
if(NOT DEFINED NAMED_LANGUAGE_SUPPORT AND ${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16")
  set(NAMED_LANGUAGE_SUPPORT ON)
  message(STATUS "[DEFAULTS] Using explicit named language support! E.g., enable_language(CXX) is needed in the project files.")
elseif(NOT DEFINED NAMED_LANGUAGE_SUPPORT AND ${CMAKE_VERSION} VERSION_LESS "3.16")
  set(NAMED_LANGUAGE_SUPPORT OFF)
  message(STATUS "[DEFAULTS] Disabling explicit named language support. Falling back to legacy behavior.")
elseif(DEFINED NAMED_LANGUAGE_SUPPORT AND ${CMAKE_VERSION} VERSION_LESS "3.16")
  message(FATAL_ERROR "CMake named language support for OBJC and OBJCXX was added in CMake 3.16.")
endif()
set(NAMED_LANGUAGE_SUPPORT_INT ${NAMED_LANGUAGE_SUPPORT} CACHE BOOL
        "Whether or not to enable explicit named language support" FORCE)

# Specify the minimum version of the deployment target.
if(NOT DEFINED DEPLOYMENT_TARGET)
  # Unless specified, SDK version 11.0 is used by default as the minimum target version (iOS, tvOS).
  set(DEPLOYMENT_TARGET "11.0")
  message(STATUS "[DEFAULTS] Using the default min-version since DEPLOYMENT_TARGET not provided!")
elseif(DEFINED DEPLOYMENT_TARGET AND PLATFORM MATCHES "^MAC_CATALYST" AND ${DEPLOYMENT_TARGET} VERSION_LESS "13.1")
  message(FATAL_ERROR "Mac Catalyst builds requires a minimum deployment target of 13.1!")
endif()

# Store the DEPLOYMENT_TARGET in the cache
set(DEPLOYMENT_TARGET "${DEPLOYMENT_TARGET}" CACHE INTERNAL "")

if(DEFINED ARCHS)
  string(REPLACE ";" "-" ARCHS_SPLIT "${ARCHS}")
endif()

set(SDK_NAME appletvos)
if(NOT ARCHS)
  set(ARCHS arm64 x86_64)
  set(APPLE_TARGET_TRIPLE_INT arm64-x86_64-apple-tvos${DEPLOYMENT_TARGET})
  set(CMAKE_XCODE_ATTRIBUTE_ARCHS[sdk=appletvos*] "arm64")
  set(CMAKE_XCODE_ATTRIBUTE_ARCHS[sdk=appletvsimulator*] "x86_64")
  set(CMAKE_XCODE_ATTRIBUTE_VALID_ARCHS[sdk=appletvos*] "arm64")
  set(CMAKE_XCODE_ATTRIBUTE_VALID_ARCHS[sdk=appletvsimulator*] "x86_64")
else()
  set(APPLE_TARGET_TRIPLE_INT ${ARCHS_SPLIT}-apple-tvos${DEPLOYMENT_TARGET})
endif()

string(REPLACE ";" " " ARCHS_SPACED "${ARCHS}")

set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${DEPLOYMENT_TARGET}")
if(NOT PLATFORM_INT MATCHES ".*COMBINED")
  set(CMAKE_XCODE_ATTRIBUTE_ARCHS[sdk=${SDK_NAME}*] "${ARCHS_SPACED}")
  set(CMAKE_XCODE_ATTRIBUTE_VALID_ARCHS[sdk=${SDK_NAME}*] "${ARCHS_SPACED}")
endif()

# If the user did not specify the SDK root to use, then query xcodebuild for it.
if(DEFINED CMAKE_OSX_SYSROOT_INT)
  # Environment variables are always preserved.
  set(ENV{_CMAKE_OSX_SYSROOT_INT} "${CMAKE_OSX_SYSROOT_INT}")
elseif(DEFINED ENV{_CMAKE_OSX_SYSROOT_INT})
  set(CMAKE_OSX_SYSROOT_INT "$ENV{_CMAKE_OSX_SYSROOT_INT}")
elseif(NOT DEFINED CMAKE_OSX_SYSROOT_INT)
  execute_process(COMMAND ${XCODEBUILD_EXECUTABLE} -version -sdk ${SDK_NAME} Path
          OUTPUT_VARIABLE CMAKE_OSX_SYSROOT_INT
          ERROR_QUIET
          OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

if (NOT DEFINED CMAKE_OSX_SYSROOT_INT AND NOT DEFINED CMAKE_OSX_SYSROOT)
  message(SEND_ERROR "Please make sure that Xcode is installed and that the toolchain"
          "is pointing to the correct path. Please run:"
          "sudo xcode-select -s /Applications/Xcode.app/Contents/Developer"
          "and see if that fixes the problem for you.")
  message(FATAL_ERROR "Invalid CMAKE_OSX_SYSROOT: ${CMAKE_OSX_SYSROOT} "
          "does not exist.")
elseif(DEFINED CMAKE_OSX_SYSROOT_INT)
  set(CMAKE_OSX_SYSROOT_INT "${CMAKE_OSX_SYSROOT_INT}" CACHE INTERNAL "")
  # Specify the location or name of the platform SDK to be used in CMAKE_OSX_SYSROOT.
  set(CMAKE_OSX_SYSROOT "${CMAKE_OSX_SYSROOT_INT}" CACHE INTERNAL "")
endif()

# Use bitcode or not
if(NOT DEFINED ENABLE_BITCODE AND NOT ARCHS MATCHES "((^|;|, )(i386|x86_64))+")
  # Unless specified, enable bitcode support by default
  message(STATUS "[DEFAULTS] Enabling bitcode support by default. ENABLE_BITCODE not provided!")
  set(ENABLE_BITCODE ON)
elseif(NOT DEFINED ENABLE_BITCODE)
  message(STATUS "[DEFAULTS] Disabling bitcode support by default on simulators. ENABLE_BITCODE not provided for override!")
  set(ENABLE_BITCODE OFF)
endif()
set(ENABLE_BITCODE_INT ${ENABLE_BITCODE} CACHE BOOL
        "Whether or not to enable bitcode" FORCE)
# Use hidden visibility or not
if(NOT DEFINED ENABLE_VISIBILITY)
  # Unless specified, disable symbols visibility by default
  set(ENABLE_VISIBILITY OFF)
  message(STATUS "[DEFAULTS] Hiding symbols visibility by default. ENABLE_VISIBILITY not provided!")
endif()
set(ENABLE_VISIBILITY_INT ${ENABLE_VISIBILITY} CACHE BOOL "Whether or not to hide symbols from the dynamic linker (-fvisibility=hidden)" FORCE)
# Set strict compiler checks or not
if(NOT DEFINED ENABLE_STRICT_TRY_COMPILE)
  # Unless specified, disable strict try_compile()
  set(ENABLE_STRICT_TRY_COMPILE OFF)
  message(STATUS "[DEFAULTS] Using NON-strict compiler checks by default. ENABLE_STRICT_TRY_COMPILE not provided!")
endif()
set(ENABLE_STRICT_TRY_COMPILE_INT ${ENABLE_STRICT_TRY_COMPILE} CACHE BOOL
        "Whether or not to use strict compiler checks" FORCE)

# Get the SDK version information.
if(DEFINED SDK_VERSION)
  # Environment variables are always preserved.
  set(ENV{_SDK_VERSION} "${SDK_VERSION}")
elseif(DEFINED ENV{_SDK_VERSION})
  set(SDK_VERSION "$ENV{_SDK_VERSION}")
elseif(NOT DEFINED SDK_VERSION)
  execute_process(COMMAND ${XCODEBUILD_EXECUTABLE} -sdk ${CMAKE_OSX_SYSROOT_INT} -version SDKVersion
          OUTPUT_VARIABLE SDK_VERSION
          ERROR_QUIET
          OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

# Find the Developer root for the specific iOS platform being compiled for
# from CMAKE_OSX_SYSROOT.  Should be ../../ from SDK specified in
# CMAKE_OSX_SYSROOT. There does not appear to be a direct way to obtain
# this information from xcrun or xcodebuild.
if (NOT DEFINED CMAKE_DEVELOPER_ROOT AND NOT CMAKE_GENERATOR MATCHES "Xcode")
  get_filename_component(PLATFORM_SDK_DIR ${CMAKE_OSX_SYSROOT_INT} PATH)
  get_filename_component(CMAKE_DEVELOPER_ROOT ${PLATFORM_SDK_DIR} PATH)
  if (NOT EXISTS "${CMAKE_DEVELOPER_ROOT}")
    message(FATAL_ERROR "Invalid CMAKE_DEVELOPER_ROOT: ${CMAKE_DEVELOPER_ROOT} does not exist.")
  endif()
endif()

# Find the C & C++ compilers for the specified SDK.
if(DEFINED CMAKE_C_COMPILER)
  # Environment variables are always preserved.
  set(ENV{_CMAKE_C_COMPILER} "${CMAKE_C_COMPILER}")
elseif(DEFINED ENV{_CMAKE_C_COMPILER})
  set(CMAKE_C_COMPILER "$ENV{_CMAKE_C_COMPILER}")
  set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
elseif(NOT DEFINED CMAKE_C_COMPILER)
  execute_process(COMMAND xcrun -sdk ${CMAKE_OSX_SYSROOT_INT} -find clang
          OUTPUT_VARIABLE CMAKE_C_COMPILER
          ERROR_QUIET
          OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
endif()
if(DEFINED CMAKE_CXX_COMPILER)
  # Environment variables are always preserved.
  set(ENV{_CMAKE_CXX_COMPILER} "${CMAKE_CXX_COMPILER}")
elseif(DEFINED ENV{_CMAKE_CXX_COMPILER})
  set(CMAKE_CXX_COMPILER "$ENV{_CMAKE_CXX_COMPILER}")
elseif(NOT DEFINED CMAKE_CXX_COMPILER)
  execute_process(COMMAND xcrun -sdk ${CMAKE_OSX_SYSROOT_INT} -find clang++
          OUTPUT_VARIABLE CMAKE_CXX_COMPILER
          ERROR_QUIET
          OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
# Find (Apple's) libtool.
if(DEFINED BUILD_LIBTOOL)
  # Environment variables are always preserved.
  set(ENV{_BUILD_LIBTOOL} "${BUILD_LIBTOOL}")
elseif(DEFINED ENV{_BUILD_LIBTOOL})
  set(BUILD_LIBTOOL "$ENV{_BUILD_LIBTOOL}")
elseif(NOT DEFINED BUILD_LIBTOOL)
  execute_process(COMMAND xcrun -sdk ${CMAKE_OSX_SYSROOT_INT} -find libtool
          OUTPUT_VARIABLE BUILD_LIBTOOL
          ERROR_QUIET
          OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
# Find the toolchain's provided install_name_tool if none is found on the host
if(DEFINED CMAKE_INSTALL_NAME_TOOL)
  # Environment variables are always preserved.
  set(ENV{_CMAKE_INSTALL_NAME_TOOL} "${CMAKE_INSTALL_NAME_TOOL}")
elseif(DEFINED ENV{_CMAKE_INSTALL_NAME_TOOL})
  set(CMAKE_INSTALL_NAME_TOOL "$ENV{_CMAKE_INSTALL_NAME_TOOL}")
elseif(NOT DEFINED CMAKE_INSTALL_NAME_TOOL)
  execute_process(COMMAND xcrun -sdk ${CMAKE_OSX_SYSROOT_INT} -find install_name_tool
          OUTPUT_VARIABLE CMAKE_INSTALL_NAME_TOOL_INT
          ERROR_QUIET
          OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(CMAKE_INSTALL_NAME_TOOL ${CMAKE_INSTALL_NAME_TOOL_INT} CACHE INTERNAL "")
endif()

# Configure libtool to be used instead of ar + ranlib to build static libraries.
# This is required on Xcode 7+, but should also work on previous versions of
# Xcode.
get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)
foreach(lang ${languages})
  set(CMAKE_${lang}_CREATE_STATIC_LIBRARY "${BUILD_LIBTOOL} -static -o <TARGET> <LINK_FLAGS> <OBJECTS> " CACHE INTERNAL "")
endforeach()

# CMake 3.14+ support building for iOS, watchOS, and tvOS out of the box.
if(MODERN_CMAKE)
  if(SDK_NAME MATCHES "iphone")
    set(CMAKE_SYSTEM_NAME iOS)
  elseif(SDK_NAME MATCHES "macosx")
    set(CMAKE_SYSTEM_NAME Darwin)
  elseif(SDK_NAME MATCHES "appletv")
    set(CMAKE_SYSTEM_NAME tvOS)
  elseif(SDK_NAME MATCHES "watch")
    set(CMAKE_SYSTEM_NAME watchOS)
  endif()
  # Provide flags for a combined FAT library build on newer CMake versions
  set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")
  set(CMAKE_IOS_INSTALL_COMBINED YES)
elseif(NOT DEFINED CMAKE_SYSTEM_NAME AND ${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.10")
  # Legacy code path prior to CMake 3.14 or fallback if no CMAKE_SYSTEM_NAME specified
  set(CMAKE_SYSTEM_NAME iOS)
elseif(NOT DEFINED CMAKE_SYSTEM_NAME)
  # Legacy code path before CMake 3.14 or fallback if no CMAKE_SYSTEM_NAME specified
  set(CMAKE_SYSTEM_NAME Darwin)
endif()
# Standard settings.
set(CMAKE_SYSTEM_VERSION ${SDK_VERSION} CACHE INTERNAL "")
set(UNIX ON CACHE BOOL "")
set(APPLE ON CACHE BOOL "")
set(IOS ON CACHE BOOL "")
# Set the architectures for which to build.
set(CMAKE_OSX_ARCHITECTURES ${ARCHS} CACHE INTERNAL "")
# Change the type of target generated for try_compile() so it'll work when cross-compiling, weak compiler checks
if(NOT ENABLE_STRICT_TRY_COMPILE_INT)
  set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()
# All iOS/Darwin specific settings - some may be redundant.
if (NOT DEFINED CMAKE_MACOSX_BUNDLE)
  set(CMAKE_MACOSX_BUNDLE YES)
endif()
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
set(CMAKE_SHARED_LIBRARY_SUFFIX ".dylib")
set(CMAKE_SHARED_MODULE_PREFIX "lib")
set(CMAKE_SHARED_MODULE_SUFFIX ".so")
set(CMAKE_C_COMPILER_ABI ELF)
set(CMAKE_CXX_COMPILER_ABI ELF)
set(CMAKE_C_HAS_ISYSROOT 1)
set(CMAKE_CXX_HAS_ISYSROOT 1)
set(CMAKE_MODULE_EXISTS 1)
set(CMAKE_DL_LIBS "")
set(CMAKE_C_OSX_COMPATIBILITY_VERSION_FLAG "-compatibility_version ")
set(CMAKE_C_OSX_CURRENT_VERSION_FLAG "-current_version ")
set(CMAKE_CXX_OSX_COMPATIBILITY_VERSION_FLAG "${CMAKE_C_OSX_COMPATIBILITY_VERSION_FLAG}")
set(CMAKE_CXX_OSX_CURRENT_VERSION_FLAG "${CMAKE_C_OSX_CURRENT_VERSION_FLAG}")

if(ARCHS MATCHES "((^|;|, )(arm64|arm64e|x86_64))+")
  set(CMAKE_C_SIZEOF_DATA_PTR 8)
  set(CMAKE_CXX_SIZEOF_DATA_PTR 8)
  if(ARCHS MATCHES "((^|;|, )(arm64|arm64e))+")
    set(CMAKE_SYSTEM_PROCESSOR "aarch64")
  else()
    set(CMAKE_SYSTEM_PROCESSOR "x86_64")
  endif()
else()
  set(CMAKE_C_SIZEOF_DATA_PTR 4)
  set(CMAKE_CXX_SIZEOF_DATA_PTR 4)
  set(CMAKE_SYSTEM_PROCESSOR "arm")
endif()

set(CMAKE_OSX_DEPLOYMENT_TARGET ${DEPLOYMENT_TARGET})

if(DEFINED APPLE_TARGET_TRIPLE_INT)
  set(APPLE_TARGET_TRIPLE ${APPLE_TARGET_TRIPLE_INT} CACHE INTERNAL "")
  set(CMAKE_C_COMPILER_TARGET ${APPLE_TARGET_TRIPLE})
  set(CMAKE_CXX_COMPILER_TARGET ${APPLE_TARGET_TRIPLE})
  set(CMAKE_ASM_COMPILER_TARGET ${APPLE_TARGET_TRIPLE})
endif()

if(ENABLE_BITCODE_INT)
  set(BITCODE "-fembed-bitcode")
  set(CMAKE_XCODE_ATTRIBUTE_BITCODE_GENERATION_MODE "bitcode")
  set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "YES")
else()
  set(BITCODE "")
  set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif()

if(NAMED_LANGUAGE_SUPPORT_INT)
  set(OBJC_VARS "-fobjc-abi-version=2 -DOBJC_OLD_DISPATCH_PROTOTYPES=0")
  set(OBJC_LEGACY_VARS "")
else()
  set(OBJC_VARS "")
  set(OBJC_LEGACY_VARS "-fobjc-abi-version=2 -DOBJC_OLD_DISPATCH_PROTOTYPES=0")
endif()

if(NOT ENABLE_VISIBILITY_INT)
  foreach(lang ${languages})
    set(CMAKE_${lang}_VISIBILITY_PRESET "hidden" CACHE INTERNAL "")
  endforeach()
  set(CMAKE_XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "YES")
  set(VISIBILITY "-fvisibility=hidden -fvisibility-inlines-hidden")
else()
  foreach(lang ${languages})
    set(CMAKE_${lang}_VISIBILITY_PRESET "default" CACHE INTERNAL "")
  endforeach()
  set(CMAKE_XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN "NO")
  set(VISIBILITY "-fvisibility=default")
endif()

if(DEFINED APPLE_TARGET_TRIPLE)
  set(APPLE_TARGET_TRIPLE_FLAG "-target ${APPLE_TARGET_TRIPLE}")
endif()

#Check if Xcode generator is used since that will handle these flags automagically
if(CMAKE_GENERATOR MATCHES "Xcode")
  message(STATUS "Not setting any manual command-line buildflags, since Xcode is selected as the generator. Modifying the Xcode build-settings directly instead.")
else()
  set(CMAKE_C_FLAGS "${C_TARGET_FLAGS} ${APPLE_TARGET_TRIPLE_FLAG} ${SDK_NAME_VERSION_FLAGS} ${OBJC_LEGACY_VARS} ${BITCODE} ${VISIBILITY} ${CMAKE_C_FLAGS}")
  set(CMAKE_C_FLAGS_DEBUG "-O0 -g ${CMAKE_C_FLAGS_DEBUG}")
  set(CMAKE_C_FLAGS_MINSIZEREL "-DNDEBUG -Os ${CMAKE_C_FLAGS_MINSIZEREL}")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "-DNDEBUG -O2 -g ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
  set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O3 ${CMAKE_C_FLAGS_RELEASE}")
  set(CMAKE_CXX_FLAGS "${C_TARGET_FLAGS} ${APPLE_TARGET_TRIPLE_FLAG} ${SDK_NAME_VERSION_FLAGS} ${OBJC_LEGACY_VARS} ${BITCODE} ${VISIBILITY} ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g ${CMAKE_CXX_FLAGS_DEBUG}")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "-DNDEBUG -Os ${CMAKE_CXX_FLAGS_MINSIZEREL}")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-DNDEBUG -O2 -g ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
  set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 ${CMAKE_CXX_FLAGS_RELEASE}")
  if(NAMED_LANGUAGE_SUPPORT_INT)
    set(CMAKE_OBJC_FLAGS "${C_TARGET_FLAGS} ${APPLE_TARGET_TRIPLE_FLAG} ${SDK_NAME_VERSION_FLAGS} ${BITCODE} ${VISIBILITY} ${OBJC_VARS} ${CMAKE_OBJC_FLAGS}")
    set(CMAKE_OBJC_FLAGS_DEBUG "-O0 -g ${CMAKE_OBJC_FLAGS_DEBUG}")
    set(CMAKE_OBJC_FLAGS_MINSIZEREL "-DNDEBUG -Os ${CMAKE_OBJC_FLAGS_MINSIZEREL}")
    set(CMAKE_OBJC_FLAGS_RELWITHDEBINFO "-DNDEBUG -O2 -g ${CMAKE_OBJC_FLAGS_RELWITHDEBINFO}")
    set(CMAKE_OBJC_FLAGS_RELEASE "-DNDEBUG -O3 ${CMAKE_OBJC_FLAGS_RELEASE}")
    set(CMAKE_OBJCXX_FLAGS "${C_TARGET_FLAGS} ${APPLE_TARGET_TRIPLE_FLAG} ${SDK_NAME_VERSION_FLAGS} ${BITCODE} ${VISIBILITY} ${OBJC_VARS} ${CMAKE_OBJCXX_FLAGS}")
    set(CMAKE_OBJCXX_FLAGS_DEBUG "-O0 -g ${CMAKE_OBJCXX_FLAGS_DEBUG}")
    set(CMAKE_OBJCXX_FLAGS_MINSIZEREL "-DNDEBUG -Os ${CMAKE_OBJCXX_FLAGS_MINSIZEREL}")
    set(CMAKE_OBJCXX_FLAGS_RELWITHDEBINFO "-DNDEBUG -O2 -g ${CMAKE_OBJCXX_FLAGS_RELWITHDEBINFO}")
    set(CMAKE_OBJCXX_FLAGS_RELEASE "-DNDEBUG -O3 ${CMAKE_OBJCXX_FLAGS_RELEASE}")
  endif()
  set(CMAKE_C_LINK_FLAGS "${C_TARGET_FLAGS} ${SDK_NAME_VERSION_FLAGS} -Wl,-search_paths_first ${CMAKE_C_LINK_FLAGS}")
  set(CMAKE_CXX_LINK_FLAGS "${C_TARGET_FLAGS} ${SDK_NAME_VERSION_FLAGS}  -Wl,-search_paths_first ${CMAKE_CXX_LINK_FLAGS}")
  if(NAMED_LANGUAGE_SUPPORT_INT)
    set(CMAKE_OBJC_LINK_FLAGS "${C_TARGET_FLAGS} ${SDK_NAME_VERSION_FLAGS} -Wl,-search_paths_first ${CMAKE_OBJC_LINK_FLAGS}")
    set(CMAKE_OBJCXX_LINK_FLAGS "${C_TARGET_FLAGS} ${SDK_NAME_VERSION_FLAGS} -Wl,-search_paths_first ${CMAKE_OBJCXX_LINK_FLAGS}")
  endif()
  set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -arch ${CMAKE_OSX_ARCHITECTURES} ${APPLE_TARGET_TRIPLE_FLAG}")
endif()

## Print status messages to inform of the current state
message(STATUS "Configuring ${SDK_NAME} build for platform: ${PLATFORM_INT}, architecture(s): ${ARCHS}")
message(STATUS "Using SDK: ${CMAKE_OSX_SYSROOT_INT}")
message(STATUS "Using C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Using CXX compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Using libtool: ${BUILD_LIBTOOL}")
message(STATUS "Using install name tool: ${CMAKE_INSTALL_NAME_TOOL}")
if(DEFINED APPLE_TARGET_TRIPLE)
  message(STATUS "Autoconf target triple: ${APPLE_TARGET_TRIPLE}")
endif()
message(STATUS "Using minimum deployment version: ${DEPLOYMENT_TARGET}"
        " (SDK version: ${SDK_VERSION})")
if(MODERN_CMAKE)
  message(STATUS "Merging integrated CMake 3.14+ iOS,tvOS,watchOS,macOS toolchain(s) with this toolchain!")
  if(PLATFORM_INT MATCHES ".*COMBINED")
    message(STATUS "Will combine built (static) artifacts into FAT lib...")
  endif()
endif()
if(CMAKE_GENERATOR MATCHES "Xcode")
  message(STATUS "Using Xcode version: ${XCODE_VERSION_INT}")
endif()
message(STATUS "CMake version: ${CMAKE_VERSION}")
if(DEFINED SDK_NAME_VERSION_FLAGS)
  message(STATUS "Using version flags: ${SDK_NAME_VERSION_FLAGS}")
endif()
message(STATUS "Using a data_ptr size of: ${CMAKE_CXX_SIZEOF_DATA_PTR}")
if(ENABLE_BITCODE_INT)
  message(STATUS "Bitcode: Enabled")
else()
  message(STATUS "Bitcode: Disabled")
endif()

if(ENABLE_VISIBILITY_INT)
  message(STATUS "Hiding symbols: Disabled")
else()
  message(STATUS "Hiding symbols: Enabled")
endif()

# Set global properties
set_property(GLOBAL PROPERTY PLATFORM "${PLATFORM}")
set_property(GLOBAL PROPERTY APPLE_TARGET_TRIPLE "${APPLE_TARGET_TRIPLE_INT}")
set_property(GLOBAL PROPERTY SDK_VERSION "${SDK_VERSION}")
set_property(GLOBAL PROPERTY XCODE_VERSION "${XCODE_VERSION_INT}")
set_property(GLOBAL PROPERTY OSX_ARCHITECTURES "${CMAKE_OSX_ARCHITECTURES}")

# Export configurable variables for the try_compile() command.
set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES
        PLATFORM
        XCODE_VERSION_INT
        SDK_VERSION
        NAMED_LANGUAGE_SUPPORT
        DEPLOYMENT_TARGET
        CMAKE_DEVELOPER_ROOT
        CMAKE_OSX_SYSROOT_INT
        ENABLE_BITCODE
        CMAKE_ASM_COMPILER
        CMAKE_C_COMPILER
        CMAKE_C_COMPILER_TARGET
        CMAKE_CXX_COMPILER
        CMAKE_CXX_COMPILER_TARGET
        BUILD_LIBTOOL
        CMAKE_INSTALL_NAME_TOOL
        CMAKE_C_FLAGS
        CMAKE_C_DEBUG
        CMAKE_C_MINSIZEREL
        CMAKE_C_RELWITHDEBINFO
        CMAKE_C_RELEASE
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_LINK_FLAGS
        CMAKE_CXX_LINK_FLAGS
        CMAKE_ASM_FLAGS
)

if(NAMED_LANGUAGE_SUPPORT_INT)
  list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES
        CMAKE_OBJC_FLAGS
        CMAKE_OBJC_DEBUG
        CMAKE_OBJC_MINSIZEREL
        CMAKE_OBJC_RELWITHDEBINFO
        CMAKE_OBJC_RELEASE
        CMAKE_OBJCXX_FLAGS
        CMAKE_OBJCXX_DEBUG
        CMAKE_OBJCXX_MINSIZEREL
        CMAKE_OBJCXX_RELWITHDEBINFO
        CMAKE_OBJCXX_RELEASE
        CMAKE_OBJC_LINK_FLAGS
        CMAKE_OBJCXX_LINK_FLAGS
  )
endif()

set(CMAKE_PLATFORM_HAS_INSTALLNAME 1)
set(CMAKE_SHARED_LINKER_FLAGS "-rpath @executable_path/Frameworks -rpath @loader_path/Frameworks")
set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "-dynamiclib -Wl,-headerpad_max_install_names")
set(CMAKE_SHARED_MODULE_CREATE_C_FLAGS "-bundle -Wl,-headerpad_max_install_names")
set(CMAKE_SHARED_MODULE_LOADER_C_FLAG "-Wl,-bundle_loader,")
set(CMAKE_SHARED_MODULE_LOADER_CXX_FLAG "-Wl,-bundle_loader,")
set(CMAKE_FIND_LIBRARY_SUFFIXES ".tbd" ".dylib" ".so" ".a")
set(CMAKE_SHARED_LIBRARY_SONAME_C_FLAG "-install_name")

# Set the find root to the SDK developer roots.
# Note: CMAKE_FIND_ROOT_PATH is only useful when cross-compiling. Thus, do not set on macOS builds.
if(NOT PLATFORM_INT MATCHES "^MAC.*$")
  list(APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_OSX_SYSROOT_INT}" CACHE INTERNAL "")
  set(CMAKE_IGNORE_PATH "/System/Library/Frameworks;/usr/local/lib" CACHE INTERNAL "")
endif()

# Default to searching for frameworks first.
set(CMAKE_FIND_FRAMEWORK FIRST)

# Set up the default search directories for frameworks.
if(PLATFORM_INT MATCHES "^MAC_CATALYST")
  set(CMAKE_FRAMEWORK_PATH
          ${CMAKE_DEVELOPER_ROOT}/Library/PrivateFrameworks
          ${CMAKE_OSX_SYSROOT_INT}/System/Library/Frameworks
          ${CMAKE_OSX_SYSROOT_INT}/System/iOSSupport/System/Library/Frameworks
          ${CMAKE_FRAMEWORK_PATH} CACHE INTERNAL "")
else()
  set(CMAKE_FRAMEWORK_PATH
          ${CMAKE_DEVELOPER_ROOT}/Library/PrivateFrameworks
          ${CMAKE_OSX_SYSROOT_INT}/System/Library/Frameworks
          ${CMAKE_FRAMEWORK_PATH} CACHE INTERNAL "")
endif()

# By default, search both the specified iOS SDK and the remainder of the host filesystem.
if(NOT CMAKE_FIND_ROOT_PATH_MODE_PROGRAM)
  set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH CACHE INTERNAL "")
endif()
if(NOT CMAKE_FIND_ROOT_PATH_MODE_LIBRARY)
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH CACHE INTERNAL "")
endif()
if(NOT CMAKE_FIND_ROOT_PATH_MODE_INCLUDE)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH CACHE INTERNAL "")
endif()
if(NOT CMAKE_FIND_ROOT_PATH_MODE_PACKAGE)
  set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH CACHE INTERNAL "")
endif()
