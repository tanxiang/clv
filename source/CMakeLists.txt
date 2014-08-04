set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -g")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(HB harfbuzz)
find_package(LibClang REQUIRED)

include_directories(
	${CMAKE_SOURCE_DIR}/include/
)

include_directories(
	SYSTEM
	${GTKMM_INCLUDE_DIRS}
	${HB_INCLUDE_DIRS}
	${LibClang_INCLUDE_DIRS}
)

add_executable(clvtk
	clvhb.cc 
	clvline.cc
	gtk_window.cc
	clvparser.cc
	clvbuffer.cc
	clvtkmm.cc
	clvcontext.cc
	clvview.cc
	dbg.cc
	unorder_tree.cc
	clvfont.cc
	filemap.cc
)

target_link_libraries(clvtk
	${GTKMM_LIBRARIES}
	${HB_LIBRARIES}
)


add_library(clvclang SHARED
	clvparser.cc
	clthread.cc
)
target_compile_options(clvclang PRIVATE -fno-rtti -fno-exceptions -fpic)
target_link_libraries(clvclang
	${LibClang_LIBRARIES}
)

