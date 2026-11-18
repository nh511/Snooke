string(RANDOM LENGTH 10 ALPHABET 0123456789 suffix)
set(out "${OUTPUT}/${MODE}-${CONFIG}-${suffix}")
execute_process(COMMAND "${SIMULATE}" "${SOURCE}/data/bundles/six-pocket-engineering.bundle"
    "${SOURCE}/data/baselines/${MODE}/initial.snapshot" "${SOURCE}/data/baselines/${MODE}/center.input" "${out}"
    RESULT_VARIABLE code OUTPUT_VARIABLE log ERROR_VARIABLE error)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "spin simulation failed: ${log}; ${error}")
endif()
execute_process(COMMAND "${NODE}" "${SOURCE}/tools/compare.cjs"
    "${SOURCE}/data/baselines/${MODE}/trajectory.csv" "${out}/trajectory.csv" 1e-9
    RESULT_VARIABLE code OUTPUT_VARIABLE log ERROR_VARIABLE error)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "spin baseline mismatch: ${log}; ${error}")
endif()
execute_process(COMMAND "${NODE}" "${SOURCE}/tests/spin-baseline.cjs" "${out}" "${MODE}"
    RESULT_VARIABLE code OUTPUT_VARIABLE log ERROR_VARIABLE error)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "spin physics assertion failed: ${log}; ${error}")
endif()
execute_process(COMMAND "${REPLAY}" "${out}/record.shot" RESULT_VARIABLE code OUTPUT_VARIABLE log ERROR_VARIABLE error)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "spin replay failed: ${log}; ${error}")
endif()
