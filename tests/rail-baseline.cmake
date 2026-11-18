string(RANDOM LENGTH 10 ALPHABET 0123456789 suffix)
set(out "${OUTPUT}/${MODE}-${CONFIG}-${suffix}")
execute_process(COMMAND "${SIMULATE}" "${SOURCE}/data/bundles/six-pocket-engineering.bundle"
    "${SOURCE}/data/baselines/${MODE}/initial.snapshot" "${SOURCE}/data/baselines/${MODE}/center.input" "${out}"
    RESULT_VARIABLE code OUTPUT_VARIABLE log ERROR_VARIABLE error)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "rail simulation failed: ${log}; ${error}")
endif()
execute_process(COMMAND "${NODE}" "${SOURCE}/tools/compare.cjs"
    "${SOURCE}/data/baselines/${MODE}/trajectory.csv" "${out}/trajectory.csv" 1e-9 RESULT_VARIABLE code)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "rail baseline mismatch")
endif()
execute_process(COMMAND "${CHECK_RAIL}" "${out}/record.shot" "${MODE}" RESULT_VARIABLE code)
if(NOT code EQUAL 0)
    message(FATAL_ERROR "rail contact/FPS/replay assertion failed")
endif()
