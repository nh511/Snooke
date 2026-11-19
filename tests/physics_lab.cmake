set(out "${OUTPUT}/physics-lab-${CONFIG}")
execute_process(COMMAND "${CMAKE_COMMAND}" -E remove_directory "${out}")
execute_process(COMMAND "${LAB}" "${SOURCE}/data/bundles/six-pocket-engineering.bundle" "${out}"
    RESULT_VARIABLE code OUTPUT_VARIABLE tool_output ERROR_VARIABLE err)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "physics lab failed: ${tool_output} ${err}")
endif()
foreach(scenario IN ITEMS single-ball ball-collision cushion pocket)
    execute_process(COMMAND "${SIMULATE}" "${SOURCE}/data/bundles/six-pocket-engineering.bundle"
        "${out}/${scenario}/initial.snapshot" "${out}/${scenario}/shot.input" "${out}/runs/${scenario}"
        RESULT_VARIABLE code OUTPUT_VARIABLE simulation_output ERROR_VARIABLE err)
    if(NOT code EQUAL 0)
        message(FATAL_ERROR "physics lab scenario ${scenario} failed: ${simulation_output} ${err}")
    endif()
endforeach()
execute_process(COMMAND "${NODE}" "${SOURCE}/tests/physics-lab.cjs" "${out}" "${out}/runs"
    RESULT_VARIABLE code OUTPUT_VARIABLE validation_output ERROR_VARIABLE err)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "physics lab validation failed: ${validation_output} ${err}")
endif()
