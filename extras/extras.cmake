# include_directories(${extra_directories})
# fill ${EXTRA_LIBRARIES} varible
include_directories(extras/pointviewer/include/)
include_directories(extras/threadholder/include/)
file(GLOB_RECURSE EXTRA_LIBRARIES extras/pointviewer/lib/*.lib)
link_directories(extras/pointviewer/lib)