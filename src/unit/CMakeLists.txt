
include_directories (
    "${CMAKE_CURRENT_LIST_DIR}"
    )

target_sources(atsdb
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/unit.h"
        "${CMAKE_CURRENT_LIST_DIR}/unitmanager.h"
        "${CMAKE_CURRENT_LIST_DIR}/dimension.h"
        "${CMAKE_CURRENT_LIST_DIR}/unitselectionwidget.h"
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/dimension.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/unitselectionwidget.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/unitmanager.cpp"
)


