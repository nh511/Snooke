set(out "${OUTPUT}/center-${CONFIG}")
string(RANDOM LENGTH 10 ALPHABET 0123456789 suffix)
set(out "${out}-${suffix}")
foreach(kind IN ITEMS center changed)
    execute_process(COMMAND "${SIMULATE}" "${SOURCE}/data/bundles/six-pocket-engineering.bundle"
        "${SOURCE}/data/baselines/center/initial.snapshot" "${SOURCE}/data/baselines/center/${kind}.input" "${out}/${kind}"
        RESULT_VARIABLE code OUTPUT_VARIABLE log ERROR_VARIABLE error)
    if(NOT code EQUAL 0)
        message(FATAL_ERROR "center simulation failed: ${log}; ${error}")
    endif()
endforeach()
execute_process(COMMAND "${SIMULATE}" "${SOURCE}/data/bundles/six-pocket-engineering.bundle"
    "${SOURCE}/data/baselines/center/initial.snapshot" "${SOURCE}/data/baselines/center/center.input" "${out}/repeat"
    RESULT_VARIABLE repeat_code OUTPUT_VARIABLE repeat_log ERROR_VARIABLE repeat_error)
if(NOT repeat_code EQUAL 0)
    message(FATAL_ERROR "repeat run failed: ${repeat_log}; ${repeat_error}")
endif()
execute_process(COMMAND "${NODE}" "${SOURCE}/tests/compare.cjs" "${out}/center" "${out}/repeat"
    RESULT_VARIABLE exact_code OUTPUT_VARIABLE exact_log ERROR_VARIABLE exact_error)
if(NOT exact_code EQUAL 0)
    message(FATAL_ERROR "exact comparison failed: ${exact_log}; ${exact_error}")
endif()
execute_process(COMMAND "${NODE}" "${SOURCE}/tools/compare.cjs"
    "${SOURCE}/data/baselines/center/trajectory.csv" "${out}/center/trajectory.csv" 1e-9
    RESULT_VARIABLE code OUTPUT_VARIABLE log ERROR_VARIABLE error)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "baseline mismatch: ${log}; ${error}")
endif()
execute_process(COMMAND "${NODE}" "${SOURCE}/tools/compare.cjs"
    "${out}/center/trajectory.csv" "${out}/changed/trajectory.csv" 1e-9
    RESULT_VARIABLE code OUTPUT_VARIABLE log ERROR_VARIABLE error)
if(NOT code EQUAL 1 OR NOT log MATCHES "step.*ball.*field")
    message(FATAL_ERROR "comparator did not identify perturbed input: ${code}; ${log}; ${error}")
endif()
