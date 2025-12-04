cmake_minimum_required(VERSION 3.10)
project(jngl-doxygen NONE)

# find_package(Doxygen COMPONENTS dot)
# if(DOXYGEN_FOUND)
	include(ExternalProject)
	ExternalProject_Add (
		cppreference
		URL "https://github.com/PeterFeicht/cppreference-doc/releases/download/v20250209/html-book-20250209.tar.xz"
		URL_HASH SHA256=ac50671a1f52d7f0ab0911d14450eb35e8c2f812edd0e426b2cd1e3d9db91d6f
		DOWNLOAD_NO_EXTRACT YES
		CONFIGURE_COMMAND xzcat "<DOWNLOAD_DIR>/html-book-20250209.tar.xz" | tar -xf - cppreference-doxygen-web.tag.xml
		BUILD_COMMAND rm "<DOWNLOAD_DIR>/html-book-20250209.tar.xz"
		INSTALL_COMMAND mv cppreference-doxygen-web.tag.xml "${CMAKE_CURRENT_SOURCE_DIR}"
	)
	ExternalProject_Add(doxygen-awesome-css
		GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
		GIT_TAG v2.4.1
		CONFIGURE_COMMAND ""
		BUILD_COMMAND ""
		INSTALL_COMMAND cd <SOURCE_DIR> && cp doxygen-awesome.css doxygen-awesome-sidebar-only.css "${CMAKE_CURRENT_SOURCE_DIR}"
	)
	# TODO: I have to figure this out:
	# add_custom_command(doxygen
	# 	COMMAND ${DOXYGEN_EXECUTABLE}
	# 	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	# 	DEPENDS cppreference
	# 	USES_TERMINAL
	# )
# endif()
