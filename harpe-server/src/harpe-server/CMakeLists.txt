set(INCROOT ${PROJECT_SOURCE_DIR}/include/${CMAKE_PROJECT_NAME})
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/${CMAKE_PROJECT_NAME})

configure_file(${INCROOT}/defines.hpp.in ${INCROOT}/defines.hpp @ONLY)

#ROOT files
set(ROOT_SRC
    ${SRCROOT}/functions.cpp
    ${SRCROOT}/models.cpp
)
set(ROOT_HEADER
    ${INCROOT}/defines.hpp
    ${INCROOT}/functions.hpp
    ${INCROOT}/models.hpp
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

add_executable(${CMAKE_PROJECT_NAME} main.cpp ${FILES_HEADER} ${FILES_SRC})
target_link_libraries(${CMAKE_PROJECT_NAME} ${LIBS})
