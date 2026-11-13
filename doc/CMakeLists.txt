find_program(PDFLATEX_EXECUTABLE pdflatex)
if(NOT PDFLATEX_EXECUTABLE)
    message(FATAL_ERROR
        "pdflatex is required to build the documentation. "
        "Install a LaTeX distribution or configure with "
        "-DNEOFLUXON_DOCUMENTATION=OFF.")
endif()

set(DOCUMENTATION_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/Neofluxon_User_Guide.tex")
set(CONFIGURED_DOCUMENTATION_SOURCE
    "${CMAKE_CURRENT_BINARY_DIR}/Neofluxon_User_Guide.tex")
set(NEOFLUXON_DOC_VERSION "${PROJECT_VERSION}")
configure_file(
    "${DOCUMENTATION_SOURCE}"
    "${CONFIGURED_DOCUMENTATION_SOURCE}"
    @ONLY
)

set(DOCUMENTATION_PDF
    "${CMAKE_CURRENT_BINARY_DIR}/Neofluxon_User_Guide-${PROJECT_VERSION}.pdf")

add_custom_command(
    OUTPUT "${DOCUMENTATION_PDF}"
    COMMAND "${PDFLATEX_EXECUTABLE}"
        "-jobname=Neofluxon_User_Guide-${PROJECT_VERSION}"
        -interaction=batchmode
        -halt-on-error
        "-output-directory=${CMAKE_CURRENT_BINARY_DIR}"
        "${CONFIGURED_DOCUMENTATION_SOURCE}"
    COMMAND "${PDFLATEX_EXECUTABLE}"
        "-jobname=Neofluxon_User_Guide-${PROJECT_VERSION}"
        -interaction=batchmode
        -halt-on-error
        "-output-directory=${CMAKE_CURRENT_BINARY_DIR}"
        "${CONFIGURED_DOCUMENTATION_SOURCE}"
    DEPENDS "${CONFIGURED_DOCUMENTATION_SOURCE}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Generating Neofluxon PDF documentation"
    VERBATIM
)

add_custom_target(GenerateDocumentation ALL DEPENDS "${DOCUMENTATION_PDF}")

install(FILES "${DOCUMENTATION_PDF}" DESTINATION "${CMAKE_INSTALL_DOCDIR}")
