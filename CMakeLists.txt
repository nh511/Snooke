cmake_minimum_required(VERSION 3.20)
project(Snooker LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_library(PhysicsCore STATIC
    PhysicsCore/src/Physics/PocketContact.cpp
    PhysicsCore/src/Physics/PocketState.cpp
    PhysicsCore/src/Table/TableGeometry.cpp
    PhysicsCore/src/Table/BallPlacement.cpp
    PhysicsCore/src/Physics/CushionContact.cpp
    PhysicsCore/src/Physics/CushionResponse.cpp
    PhysicsCore/src/Data/CushionProfile.cpp
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
    PhysicsCore/src/Replay/CollisionLog.cpp
    PhysicsCore/src/Shot/CueProfile.cpp
    PhysicsCore/src/Shot/CueImpulse.cpp
    PhysicsCore/src/Shot/ShotInput.cpp)
target_include_directories(PhysicsCore PUBLIC PhysicsCore/include)
target_compile_features(PhysicsCore PUBLIC cxx_std_17)
add_library(RulesCore STATIC RulesCore/src/BallValues.cpp RulesCore/src/MatchState.cpp RulesCore/src/FrameOpening.cpp RulesCore/src/BallOn.cpp)
target_include_directories(RulesCore PUBLIC RulesCore/include)
target_link_libraries(RulesCore PUBLIC PhysicsCore)
add_custom_target(Tools DEPENDS snooker_core_info snooker_validation snooker_single_ball snooker_cue_strike)
add_executable(snooker_core_info tools/core_info.cpp)
target_link_libraries(snooker_core_info PRIVATE PhysicsCore RulesCore)
enable_testing()
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
