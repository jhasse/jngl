cmake_minimum_required(VERSION 3.9)
project(jngl-doxygen NONE)

find_package(Doxygen COMPONENTS dot)
if(DOXYGEN_FOUND)
	include(ExternalProject)
	ExternalProject_Add (
		cppreference
		URL "https://github.com/PeterFeicht/cppreference-doc/releases/download/v20190602/html-book-20190602.tar.xz"
		URL_HASH SHA256=cb06cb61206cf38c6b6d978d286f6dfa05052152397af2f0759fff33be2f6efc
		DOWNLOAD_NO_EXTRACT YES
		CONFIGURE_COMMAND xzcat "<DOWNLOAD_DIR>/html-book-20190602.tar.xz" | tar -xf - cppreference-doxygen-web.tag.xml
		BUILD_COMMAND rm "<DOWNLOAD_DIR>/html-book-20190602.tar.xz"
		INSTALL_COMMAND mv cppreference-doxygen-web.tag.xml "${CMAKE_CURRENT_SOURCE_DIR}"
	)
	# TODO: I have to figure this out:
	# add_custom_command(doxygen
	# 	COMMAND ${DOXYGEN_EXECUTABLE}
	# 	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	# 	DEPENDS cppreference
	# 	USES_TERMINAL
	# )
endif()
