if(NOT DEFINED TOOL OR NOT DEFINED SOURCE OR NOT DEFINED OUT)
    message(FATAL_ERROR "bundle test arguments are incomplete")
endif()
execute_process(COMMAND "${TOOL}" "${SOURCE}/data/table-six-pocket.snooker"
    "${SOURCE}/data/profiles/cushion-standard.txt"
    "${SOURCE}/data/profiles/pocket-standard.txt"
    "${SOURCE}/data/profiles/cloth-standard.txt"
    "${SOURCE}/data/profiles/ball-contact.txt"
    "${SOURCE}/data/profiles/cue-standard.txt" "${OUT}/six-pocket-engineering.generated.bundle"
    RESULT_VARIABLE code OUTPUT_VARIABLE output ERROR_VARIABLE error)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "bundle generation failed (${code}): ${output}; ${error}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" -E compare_files
    "${SOURCE}/data/bundles/six-pocket-engineering.bundle" "${OUT}/six-pocket-engineering.generated.bundle"
    RESULT_VARIABLE compare_code)
if(NOT compare_code EQUAL 0)
    message(FATAL_ERROR "generated physics bundle differs from checked-in bundle")
endif()
