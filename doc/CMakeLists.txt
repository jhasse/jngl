cmake_minimum_required(VERSION 3.9)
project(jngl-doxygen NONE)

# find_package(Doxygen COMPONENTS dot)
# if(DOXYGEN_FOUND)
	include(ExternalProject)
	ExternalProject_Add (
		cppreference
		URL "https://github.com/PeterFeicht/cppreference-doc/releases/download/v20220730/html-book-20220730.tar.xz"
		URL_HASH SHA256=71f15003c168b8dc5a00cbaf19b6480a9b3e87ab7e462aa39edb63d7511c028b
		DOWNLOAD_NO_EXTRACT YES
		CONFIGURE_COMMAND xzcat "<DOWNLOAD_DIR>/html-book-20220730.tar.xz" | tar -xf - cppreference-doxygen-web.tag.xml
		BUILD_COMMAND rm "<DOWNLOAD_DIR>/html-book-20220730.tar.xz"
		INSTALL_COMMAND mv cppreference-doxygen-web.tag.xml "${CMAKE_CURRENT_SOURCE_DIR}"
	)
	ExternalProject_Add(doxygen-awesome-css
		GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
		GIT_TAG v2.1.0
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
