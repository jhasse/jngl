set(IOS 1)
set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_OSX_DEPLOYMENT_TARGET 12.4)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY) # Required for libogg
SET(CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")

### see https://github.com/cristeab/ios-cmake

# Setup iOS platform unless specified manually with IOS_PLATFORM
if (NOT DEFINED IOS_PLATFORM)
	set (IOS_PLATFORM "OS")
endif ()
set (IOS_PLATFORM ${IOS_PLATFORM} CACHE STRING "Type of iOS Platform")

# Check the platform selection and setup for developer root
if (${IOS_PLATFORM} STREQUAL "OS")
	set (IOS_PLATFORM_LOCATION "iPhoneOS.platform")

	# This causes the installers to properly locate the output libraries
	set (CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphoneos")
elseif (${IOS_PLATFORM} STREQUAL "SIMULATOR")
	set (IS_SIMULATOR true)
	set (IOS_PLATFORM_LOCATION "iPhoneSimulator.platform")

	# This causes the installers to properly locate the output libraries
	set (CMAKE_XCODE_EFFECTIVE_PLATFORMS "-iphonesimulator")
else ()
	message (FATAL_ERROR "Unsupported IOS_PLATFORM value selected. Please choose OS or SIMULATOR")
endif ()

# Setup iOS developer location unless specified manually with CMAKE_IOS_DEVELOPER_ROOT
# Note Xcode 4.3 changed the installation location, choose the most recent one available
exec_program(/usr/bin/xcode-select ARGS -print-path OUTPUT_VARIABLE CMAKE_XCODE_DEVELOPER_DIR)
set (XCODE_POST_43_ROOT "${CMAKE_XCODE_DEVELOPER_DIR}/Platforms/${IOS_PLATFORM_LOCATION}/Developer")
set (XCODE_PRE_43_ROOT "/Developer/Platforms/${IOS_PLATFORM_LOCATION}/Developer")
if (NOT DEFINED CMAKE_IOS_DEVELOPER_ROOT)
	if (EXISTS ${XCODE_POST_43_ROOT})
		set (CMAKE_IOS_DEVELOPER_ROOT ${XCODE_POST_43_ROOT})
	elseif(EXISTS ${XCODE_PRE_43_ROOT})
		set (CMAKE_IOS_DEVELOPER_ROOT ${XCODE_PRE_43_ROOT})
	endif (EXISTS ${XCODE_POST_43_ROOT})
endif ()
set (CMAKE_IOS_DEVELOPER_ROOT ${CMAKE_IOS_DEVELOPER_ROOT} CACHE PATH "Location of iOS Platform")

# Find and use the most recent iOS sdk unless specified manually with CMAKE_IOS_SDK_ROOT
if (NOT DEFINED CMAKE_IOS_SDK_ROOT)
	file (GLOB _CMAKE_IOS_SDKS "${CMAKE_IOS_DEVELOPER_ROOT}/SDKs/*")
	if (_CMAKE_IOS_SDKS)
		list (SORT _CMAKE_IOS_SDKS)
		list (REVERSE _CMAKE_IOS_SDKS)
		list (GET _CMAKE_IOS_SDKS 0 CMAKE_IOS_SDK_ROOT)
	else (_CMAKE_IOS_SDKS)
		message (FATAL_ERROR "No iOS SDK's found in default search path ${CMAKE_IOS_DEVELOPER_ROOT}. Manually set CMAKE_IOS_SDK_ROOT or install the iOS SDK.")
	endif (_CMAKE_IOS_SDKS)
	message (STATUS "Toolchain using default iOS SDK: ${CMAKE_IOS_SDK_ROOT}")
endif ()
set (CMAKE_IOS_SDK_ROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Location of the selected iOS SDK")

# Set the sysroot default to the most recent SDK
set (CMAKE_OSX_SYSROOT ${CMAKE_IOS_SDK_ROOT} CACHE PATH "Sysroot used for iOS support")

# set the architecture for iOS
if (${IOS_PLATFORM} STREQUAL "OS")
	set (IOS_ARCH armv7;armv7s;arm64)
elseif (${IOS_PLATFORM} STREQUAL "SIMULATOR")
	set (IOS_ARCH i386;x86_64)
endif ()
