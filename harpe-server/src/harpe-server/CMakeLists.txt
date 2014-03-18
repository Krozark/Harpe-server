set(INCROOT ${PROJECT_SOURCE_DIR}/include/${CMAKE_PROJECT_NAME})
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/${CMAKE_PROJECT_NAME})

#ROOT files
set(ROOT_SRC
    ${SRCROOT}/dispatch.cpp
    ${SRCROOT}/functions.cpp
)
set(ROOT_HEADER
    ${INCROOT}/defines.hpp
    ${INCROOT}/dispatch.hpp
    ${INCROOT}/functions.hpp
    ${INCROOT}/models.hpp
    ${INCROOT}/models.tpl
)
set(ROOT_FILES
    ${ROOT_HEADER}
    ${ROOT_SRC}
)

#FILES
set(FILES_HEADER
    ${ROOT_HEADER}
)
set(FILES_SRC
    ${ROOT_SRC}
)

find_library(LIBS ${LIBS})

add_executable(${CMAKE_PROJECT_NAME} main.cpp ${FILES_HEADER} ${FILES_SRC})
target_link_libraries(${CMAKE_PROJECT_NAME} ${LIBS})
