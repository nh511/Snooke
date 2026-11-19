set(out "${OUTPUT}/performance-${CONFIG}")
execute_process(COMMAND "${CMAKE_COMMAND}" -E remove_directory "${out}")
execute_process(COMMAND "${PERFORMANCE}" "${SOURCE}/data/bundles/six-pocket-engineering.bundle"
    "${SOURCE}/data/baselines/follow/initial.snapshot" "${SOURCE}/data/baselines/follow/center.input" "${out}"
    RESULT_VARIABLE code OUTPUT_VARIABLE tool_output ERROR_VARIABLE err)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "performance tool failed: ${tool_output} ${err}")
endif()
execute_process(COMMAND "${NODE}" "${SOURCE}/tests/performance.cjs" "${out}"
    RESULT_VARIABLE code OUTPUT_VARIABLE validation_output ERROR_VARIABLE err)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "performance validation failed: ${validation_output} ${err}")
endif()
