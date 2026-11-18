set(out "${OUTPUT}/service-cli-${CONFIG}")
if(EXISTS "${out}")
    file(GLOB old "${out}/*")
    # Keep previous runs intact; choose a fresh evidence subdirectory.
    string(RANDOM LENGTH 10 ALPHABET 0123456789 suffix)
    set(out "${out}-${suffix}")
endif()
execute_process(COMMAND "${SIMULATE}" "${SOURCE}/data/bundles/six-pocket-engineering.bundle"
    "${OUTPUT}/service-opening.snapshot" "${OUTPUT}/service-opening.input" "${out}"
    RESULT_VARIABLE code OUTPUT_VARIABLE log ERROR_VARIABLE error)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "simulation CLI failed (${code}): ${log}; ${error}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" -E compare_files "${out}/final.snapshot"
    "${OUTPUT}/service-opening-final.snapshot" RESULT_VARIABLE different)
if(NOT different EQUAL 0)
    message(FATAL_ERROR "service API/CLI final states differ")
endif()
file(SIZE "${out}/events.txt" bytes)
if(bytes LESS 100)
    message(FATAL_ERROR "CLI event sequence missing")
endif()
