cmake_minimum_required(VERSION 3.20)
project(Snooker LANGUAGES CXX)
find_program(NODE_EXECUTABLE NAMES node REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_library(PhysicsCore STATIC
    PhysicsCore/src/Physics/PocketSimulation.cpp
    PhysicsCore/src/Physics/PhysicsFault.cpp
    PhysicsCore/src/Physics/PhysicsService.cpp
    PhysicsCore/src/Physics/PocketContact.cpp
    PhysicsCore/src/Physics/PocketState.cpp
    PhysicsCore/src/Table/TableGeometry.cpp
    PhysicsCore/src/Table/BallPlacement.cpp
    PhysicsCore/src/Physics/CushionContact.cpp
    PhysicsCore/src/Physics/CushionResponse.cpp
    PhysicsCore/src/Data/CushionProfile.cpp
    PhysicsCore/src/Data/ClothProfile.cpp
    PhysicsCore/src/Data/PocketProfile.cpp
    PhysicsCore/src/Data/CoreConfig.cpp
    PhysicsCore/src/Data/TableDefinition.cpp
    PhysicsCore/src/Data/BallDefinition.cpp
    PhysicsCore/src/Physics/BallState.cpp
    PhysicsCore/src/Physics/BallPairs.cpp
    PhysicsCore/src/Physics/BallContact.cpp
    PhysicsCore/src/Physics/BallResponse.cpp
    PhysicsCore/src/Physics/Touching.cpp
    PhysicsCore/src/Physics/PhysicsSnapshot.cpp
    PhysicsCore/src/Physics/ClothFriction.cpp
    PhysicsCore/src/Physics/RollingResistance.cpp
    PhysicsCore/src/Physics/AngularUpdate.cpp
    PhysicsCore/src/Replay/ShotRecord.cpp
    PhysicsCore/src/Replay/PhysicsShotRecord.cpp
    PhysicsCore/src/Replay/ShotReplay.cpp
    PhysicsCore/src/Replay/ReplayController.cpp
    PhysicsCore/src/Replay/CollisionLog.cpp
    PhysicsCore/src/Shot/CueProfile.cpp
    PhysicsCore/src/Shot/CueImpulse.cpp
    PhysicsCore/src/Shot/ShotInput.cpp)
target_include_directories(PhysicsCore PUBLIC PhysicsCore/include)
target_compile_features(PhysicsCore PUBLIC cxx_std_17)
add_library(RulesCore STATIC RulesCore/src/BallValues.cpp RulesCore/src/MatchState.cpp RulesCore/src/FrameOpening.cpp RulesCore/src/BallOn.cpp)
target_sources(RulesCore PRIVATE RulesCore/src/ShotPhysicsResult.cpp)
target_sources(RulesCore PRIVATE RulesCore/src/ShotScoring.cpp)
target_sources(RulesCore PRIVATE RulesCore/src/CueBallFoul.cpp)
target_sources(RulesCore PRIVATE RulesCore/src/FirstContact.cpp)
target_include_directories(RulesCore PUBLIC RulesCore/include)
target_link_libraries(RulesCore PUBLIC PhysicsCore)
add_custom_target(Tools DEPENDS snooker_core_info snooker_validation snooker_single_ball snooker_cue_strike)
add_executable(snooker_pocket_bundle tools/pocket_bundle.cpp)
add_executable(snooker_fault_replay tools/fault_replay.cpp)
add_executable(snooker_simulate tools/simulate.cpp)
add_executable(snooker_replay tools/replay.cpp)
target_link_libraries(snooker_replay PRIVATE PhysicsCore)
add_dependencies(Tools snooker_replay)
add_executable(snooker_baseline_seed tools/baseline_seed.cpp)
target_link_libraries(snooker_baseline_seed PRIVATE PhysicsCore)
add_dependencies(Tools snooker_baseline_seed)
target_link_libraries(snooker_simulate PRIVATE PhysicsCore)
add_dependencies(Tools snooker_simulate)
target_link_libraries(snooker_fault_replay PRIVATE PhysicsCore)
add_dependencies(Tools snooker_fault_replay)
target_link_libraries(snooker_pocket_bundle PRIVATE PhysicsCore)
add_dependencies(Tools snooker_pocket_bundle)
add_executable(snooker_core_info tools/core_info.cpp)
target_link_libraries(snooker_core_info PRIVATE PhysicsCore RulesCore)
enable_testing()
add_executable(snooker_test_scoring tests/scoring.cpp)
target_link_libraries(snooker_test_scoring PRIVATE RulesCore)
add_test(NAME rules_scoring COMMAND snooker_test_scoring)
add_executable(snooker_test_cue_ball_foul tests/cue_ball_foul.cpp)
target_link_libraries(snooker_test_cue_ball_foul PRIVATE RulesCore)
add_test(NAME rules_cue_ball_foul COMMAND snooker_test_cue_ball_foul "${CMAKE_CURRENT_SOURCE_DIR}/data/table-standard.snooker")
add_executable(snooker_test_first_contact tests/first_contact.cpp)
target_link_libraries(snooker_test_first_contact PRIVATE RulesCore)
add_test(NAME rules_first_contact COMMAND snooker_test_first_contact)
add_executable(snooker_test_rule_input tests/rule_input.cpp)
target_link_libraries(snooker_test_rule_input PRIVATE RulesCore)
add_test(NAME rules_physics_input COMMAND snooker_test_rule_input "${CMAKE_CURRENT_SOURCE_DIR}/data/baselines/follow/record.shot")
set_tests_properties(rules_physics_input PROPERTIES TIMEOUT 60)
add_executable(snooker_test_replay_controller tests/replay_controller.cpp)
target_link_libraries(snooker_test_replay_controller PRIVATE PhysicsCore)
add_test(NAME core_replay_controller COMMAND snooker_test_replay_controller "${CMAKE_CURRENT_SOURCE_DIR}/data/baselines/center/record.shot")
set_tests_properties(core_replay_controller PROPERTIES TIMEOUT 120)
add_executable(snooker_test_pocket_baseline tests/pocket_baseline.cpp)
target_link_libraries(snooker_test_pocket_baseline PRIVATE PhysicsCore)
add_test(NAME baseline_pocket_cases COMMAND snooker_test_pocket_baseline
    "${CMAKE_CURRENT_SOURCE_DIR}/data/bundles/six-pocket-engineering.bundle" "${CMAKE_CURRENT_SOURCE_DIR}/data/baselines/pocket")
set_tests_properties(baseline_pocket_cases PROPERTIES TIMEOUT 60)
add_executable(snooker_test_rail_baseline tests/rail_baseline.cpp)
target_link_libraries(snooker_test_rail_baseline PRIVATE PhysicsCore)
add_executable(snooker_test_shot_replay tests/shot_replay.cpp)
target_link_libraries(snooker_test_shot_replay PRIVATE PhysicsCore)
add_test(NAME core_shot_replay COMMAND snooker_test_shot_replay "${CMAKE_CURRENT_SOURCE_DIR}/data/baselines/center/record.shot")
set_tests_properties(core_shot_replay PROPERTIES TIMEOUT 30)
add_test(NAME comparator_contract COMMAND "${NODE_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tests/compare.cjs")
add_executable(snooker_test_physics_record tests/physics_record.cpp)
target_link_libraries(snooker_test_physics_record PRIVATE PhysicsCore)
add_executable(snooker_test_physics_service tests/physics_service.cpp)
target_link_libraries(snooker_test_physics_service PRIVATE PhysicsCore)
add_test(NAME core_physics_service COMMAND snooker_test_physics_service "${CMAKE_CURRENT_SOURCE_DIR}/data/bundles/six-pocket-engineering.bundle")
set_tests_properties(core_physics_service PROPERTIES TIMEOUT 120)
set_tests_properties(core_physics_service PROPERTIES FIXTURES_SETUP service_files TIMEOUT 240)
add_test(NAME physics_service_cli COMMAND "${CMAKE_COMMAND}"
    "-DSIMULATE=$<TARGET_FILE:snooker_simulate>" "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}"
    "-DRECORD_TEST=$<TARGET_FILE:snooker_test_physics_record>"
    "-DNODE=${NODE_EXECUTABLE}"
    "-DREPLAY=$<TARGET_FILE:snooker_replay>"
    "-DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}" "-DCONFIG=$<CONFIG>" -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/service_cli.cmake")
set_tests_properties(physics_service_cli PROPERTIES FIXTURES_REQUIRED service_files TIMEOUT 240)
add_test(NAME baseline_center COMMAND "${CMAKE_COMMAND}"
    "-DSIMULATE=$<TARGET_FILE:snooker_simulate>" "-DNODE=${NODE_EXECUTABLE}"
    "-DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}" "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}" "-DCONFIG=$<CONFIG>"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/baseline.cmake")
set_tests_properties(baseline_center PROPERTIES TIMEOUT 60)
add_test(NAME baseline_side_spin COMMAND "${CMAKE_COMMAND}"
    "-DSIMULATE=$<TARGET_FILE:snooker_simulate>" "-DREPLAY=$<TARGET_FILE:snooker_replay>" "-DNODE=${NODE_EXECUTABLE}"
    "-DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}" "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}" "-DCONFIG=$<CONFIG>"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/side-baseline.cmake")
set_tests_properties(baseline_side_spin PROPERTIES TIMEOUT 90)
foreach(mode IN ITEMS one-rail two-rail touching-rail left)
    add_test(NAME rail_${mode} COMMAND "${CMAKE_COMMAND}"
        "-DSIMULATE=$<TARGET_FILE:snooker_simulate>" "-DCHECK_RAIL=$<TARGET_FILE:snooker_test_rail_baseline>"
        "-DNODE=${NODE_EXECUTABLE}" "-DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}" "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}"
        "-DCONFIG=$<CONFIG>" "-DMODE=${mode}" -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/rail-baseline.cmake")
    set_tests_properties(rail_${mode} PROPERTIES TIMEOUT 120)
endforeach()
foreach(mode IN ITEMS follow draw)
    add_test(NAME baseline_${mode} COMMAND "${CMAKE_COMMAND}"
        "-DSIMULATE=$<TARGET_FILE:snooker_simulate>" "-DREPLAY=$<TARGET_FILE:snooker_replay>" "-DNODE=${NODE_EXECUTABLE}"
        "-DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}" "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}" "-DCONFIG=$<CONFIG>" "-DMODE=${mode}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/spin-baseline.cmake")
    set_tests_properties(baseline_${mode} PROPERTIES TIMEOUT 60)
endforeach()
add_executable(snooker_test_pocket_simulation tests/pocket_simulation.cpp)
target_link_libraries(snooker_test_pocket_simulation PRIVATE PhysicsCore)
add_test(NAME core_pocket_simulation COMMAND snooker_test_pocket_simulation
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-standard.snooker"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cushion-standard.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/pocket-standard.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cloth-standard.txt")
set_tests_properties(core_pocket_simulation PROPERTIES TIMEOUT 30)
add_executable(snooker_test_physics_fault tests/physics_fault.cpp)
target_link_libraries(snooker_test_physics_fault PRIVATE PhysicsCore)
add_test(NAME core_physics_fault COMMAND snooker_test_physics_fault
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-standard.snooker"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cushion-standard.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/pocket-standard.txt")
set_tests_properties(core_physics_fault PROPERTIES TIMEOUT 30)
set_tests_properties(core_physics_fault PROPERTIES FIXTURES_SETUP fault_files)
foreach(fault_case IN ITEMS fault timeout eventloop)
    add_test(NAME replay_fault_${fault_case} COMMAND snooker_fault_replay "physics-${fault_case}.txt")
    set_tests_properties(replay_fault_${fault_case} PROPERTIES FIXTURES_REQUIRED fault_files TIMEOUT 30)
endforeach()
foreach(case IN ITEMS main units log math rules parsing definition state clock snapshot cloth friction rolling angular motion sleep)
    add_executable(snooker_test_${case} tests/${case}.cpp)
    target_link_libraries(snooker_test_${case} PRIVATE PhysicsCore RulesCore)
    if(case STREQUAL "log")
        add_test(NAME core_${case} COMMAND snooker_test_${case} log-test.txt)
    else()
        add_test(NAME core_${case} COMMAND snooker_test_${case})
    endif()
endforeach()
add_custom_target(Tests DEPENDS snooker_test_main snooker_test_units snooker_test_log snooker_test_math snooker_test_rules snooker_test_parsing)
add_dependencies(Tests snooker_test_pocket_simulation)
add_dependencies(Tests snooker_test_physics_fault snooker_fault_replay)
add_dependencies(Tests snooker_test_physics_service)
add_dependencies(Tests snooker_test_physics_record)
add_dependencies(Tests snooker_test_shot_replay snooker_replay)
add_dependencies(Tests snooker_test_rail_baseline)
add_dependencies(Tests snooker_test_pocket_baseline)
add_dependencies(Tests snooker_test_replay_controller)
add_dependencies(Tests snooker_test_rule_input)
add_dependencies(Tests snooker_test_scoring)
add_dependencies(Tests snooker_test_cue_ball_foul)
add_dependencies(Tests snooker_test_first_contact)
add_test(NAME UnitsFailureExit COMMAND snooker_test_units --expect-centimeters 99)
set_tests_properties(UnitsFailureExit PROPERTIES WILL_FAIL TRUE)
add_test(NAME tools_core_info COMMAND snooker_core_info)
add_executable(snooker_test_config tests/config.cpp)
target_link_libraries(snooker_test_config PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_config)
add_dependencies(Tests snooker_test_definition)
add_dependencies(Tests snooker_test_state)
add_dependencies(Tests snooker_test_clock)
add_dependencies(Tests snooker_test_snapshot)
add_dependencies(Tests snooker_test_cloth)
add_dependencies(Tests snooker_test_friction)
add_dependencies(Tests snooker_test_rolling)
add_dependencies(Tests snooker_test_angular)
add_dependencies(Tests snooker_test_motion)
add_dependencies(Tests snooker_test_sleep)
add_executable(snooker_test_shot_input tests/shot_input.cpp)
target_link_libraries(snooker_test_shot_input PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_shot_input)
add_test(NAME core_shot_input COMMAND snooker_test_shot_input)
add_executable(snooker_test_impulse tests/impulse.cpp)
target_link_libraries(snooker_test_impulse PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_impulse)
add_test(NAME core_impulse COMMAND snooker_test_impulse)
add_executable(snooker_test_pairs tests/pairs.cpp)
target_link_libraries(snooker_test_pairs PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_pairs)
add_test(NAME core_pairs COMMAND snooker_test_pairs)
add_executable(snooker_test_contact tests/contact.cpp)
target_link_libraries(snooker_test_contact PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_contact)
add_test(NAME core_contact COMMAND snooker_test_contact)
add_executable(snooker_test_response tests/response.cpp)
target_link_libraries(snooker_test_response PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_response)
add_test(NAME core_response COMMAND snooker_test_response "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/ball-contact.txt")
add_executable(snooker_test_collision_log tests/collision_log.cpp)
target_link_libraries(snooker_test_collision_log PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_collision_log)
add_test(NAME core_collision_log COMMAND snooker_test_collision_log)
add_executable(snooker_test_touching tests/touching.cpp)
target_link_libraries(snooker_test_touching PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_touching)
add_test(NAME core_touching COMMAND snooker_test_touching)
add_executable(snooker_cue_strike tools/cue_strike.cpp)
target_link_libraries(snooker_cue_strike PRIVATE PhysicsCore)
add_test(NAME shot_entry_parity COMMAND "${CMAKE_COMMAND}"
    "-DSTRIKE=$<TARGET_FILE:snooker_cue_strike>" "-DSINGLE=$<TARGET_FILE:snooker_single_ball>"
    "-DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}" "-DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}" "-DCONFIG=$<CONFIG>"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/shot_entry.cmake")
add_executable(snooker_test_record tests/record.cpp)
target_link_libraries(snooker_test_record PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_record)
add_test(NAME core_record COMMAND snooker_test_record "${CMAKE_CURRENT_SOURCE_DIR}/data/example.shot")
add_executable(snooker_test_table tests/table.cpp)
target_link_libraries(snooker_test_table PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_table)
add_test(NAME core_table COMMAND snooker_test_table "${CMAKE_CURRENT_SOURCE_DIR}/data/table-standard.snooker" "${CMAKE_CURRENT_SOURCE_DIR}/data/table-training.snooker")
add_test(NAME core_config COMMAND snooker_test_config "${CMAKE_CURRENT_SOURCE_DIR}/data/default.snooker")
add_executable(snooker_validation tools/validation.cpp)
target_link_libraries(snooker_validation PRIVATE PhysicsCore)
add_test(NAME validation_empty_math COMMAND snooker_validation --parameters "${CMAKE_CURRENT_SOURCE_DIR}/data/validation/parameters.txt" --snapshot "${CMAKE_CURRENT_SOURCE_DIR}/data/validation/empty.snapshot")
add_test(NAME validation_reject_bad_snapshot COMMAND snooker_validation --parameters "${CMAKE_CURRENT_SOURCE_DIR}/data/validation/parameters.txt" --snapshot "${CMAKE_CURRENT_SOURCE_DIR}/data/validation/invalid.snapshot")
set_tests_properties(validation_reject_bad_snapshot PROPERTIES WILL_FAIL TRUE)
add_test(NAME validation_config COMMAND snooker_validation --config "${CMAKE_CURRENT_SOURCE_DIR}/data/default.snooker")
add_executable(snooker_single_ball tools/single_ball.cpp)
target_link_libraries(snooker_single_ball PRIVATE PhysicsCore)
add_test(NAME single_ball_trajectory COMMAND snooker_single_ball "${CMAKE_CURRENT_SOURCE_DIR}/data/default.snooker" "${CMAKE_CURRENT_SOURCE_DIR}/data/validation/single-ball.snapshot" trajectory.csv final.snapshot)
add_executable(snooker_test_cushion tests/cushion.cpp)
target_link_libraries(snooker_test_cushion PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_cushion)
add_test(NAME core_cushion COMMAND snooker_test_cushion
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cushion-standard.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cushion-training.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-standard.snooker"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-training.snooker")
add_executable(snooker_test_pocket tests/pocket.cpp)
target_link_libraries(snooker_test_pocket PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_pocket)
add_test(NAME core_pocket COMMAND snooker_test_pocket
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/pocket-standard.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/pocket-training.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-standard.snooker"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-training.snooker")
add_executable(snooker_test_cue tests/cue.cpp)
target_link_libraries(snooker_test_cue PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_cue)
add_test(NAME core_cue COMMAND snooker_test_cue
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cue-standard.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cue-training.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/default.snooker")
add_executable(snooker_test_geometry tests/geometry.cpp)
add_executable(snooker_test_pocket_contact tests/pocket_contact.cpp)
add_executable(snooker_test_pocket_state tests/pocket_state.cpp)
target_link_libraries(snooker_test_pocket_state PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_pocket_state)
add_test(NAME core_pocket_state COMMAND snooker_test_pocket_state)
target_link_libraries(snooker_test_pocket_contact PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_pocket_contact)
add_test(NAME core_pocket_contact COMMAND snooker_test_pocket_contact
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-standard.snooker"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cushion-standard.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/pocket-standard.txt")
add_executable(snooker_test_ball_on tests/ball_on.cpp)
target_link_libraries(snooker_test_ball_on PRIVATE RulesCore)
add_dependencies(Tests snooker_test_ball_on)
add_test(NAME rules_ball_on COMMAND snooker_test_ball_on)
add_executable(snooker_test_frame_opening tests/frame_opening.cpp)
target_link_libraries(snooker_test_frame_opening PRIVATE RulesCore)
add_dependencies(Tests snooker_test_frame_opening)
add_test(NAME rules_frame_opening COMMAND snooker_test_frame_opening
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-opening.snooker" "${CMAKE_CURRENT_SOURCE_DIR}/data/opening-layout.txt")
add_executable(snooker_test_match_state tests/match_state.cpp)
target_link_libraries(snooker_test_match_state PRIVATE RulesCore)
add_dependencies(Tests snooker_test_match_state)
add_test(NAME rules_match_state COMMAND snooker_test_match_state)
add_executable(snooker_test_placement tests/placement.cpp)
target_link_libraries(snooker_test_placement PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_placement)
add_test(NAME core_placement COMMAND snooker_test_placement
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-opening.snooker" "${CMAKE_CURRENT_SOURCE_DIR}/data/opening-layout.txt")
add_executable(snooker_test_cushion_response tests/cushion_response.cpp)
target_link_libraries(snooker_test_cushion_response PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_cushion_response)
add_test(NAME core_cushion_response COMMAND snooker_test_cushion_response
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-standard.snooker"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cushion-standard.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/pocket-standard.txt")
add_executable(snooker_test_cushion_contact tests/cushion_contact.cpp)
target_link_libraries(snooker_test_cushion_contact PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_cushion_contact)
add_test(NAME core_cushion_contact COMMAND snooker_test_cushion_contact
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-standard.snooker"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cushion-standard.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/pocket-standard.txt")
target_link_libraries(snooker_test_geometry PRIVATE PhysicsCore)
add_dependencies(Tests snooker_test_geometry)
add_test(NAME core_geometry COMMAND snooker_test_geometry
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-standard.snooker"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cushion-standard.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/pocket-standard.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-training.snooker"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cushion-training.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/pocket-training.txt")
add_executable(snooker_table_geometry tools/table_geometry.cpp)
target_link_libraries(snooker_table_geometry PRIVATE PhysicsCore)
add_dependencies(Tools snooker_table_geometry)
foreach(preset IN ITEMS standard training)
    add_test(NAME geometry_export_${preset} COMMAND snooker_table_geometry
        "${CMAKE_CURRENT_SOURCE_DIR}/data/table-${preset}.snooker"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cushion-${preset}.txt"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/pocket-${preset}.txt"
        "geometry-${preset}.svg")
endforeach()
add_test(NAME core_six_pocket_geometry COMMAND snooker_table_geometry
    "${CMAKE_CURRENT_SOURCE_DIR}/data/table-six-pocket.snooker"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/cushion-standard.txt"
    "${CMAKE_CURRENT_SOURCE_DIR}/data/profiles/pocket-standard.txt"
    "six-pocket.svg")
add_test(NAME physics_bundle_byte_match COMMAND "${CMAKE_COMMAND}"
    "-DTOOL=$<TARGET_FILE:snooker_pocket_bundle>"
    "-DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}"
    "-DOUT=${CMAKE_CURRENT_BINARY_DIR}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/bundle.cmake")
