add_executable(protocol_tests
    protocol_tests.cpp
)
target_link_libraries(protocol_tests PRIVATE tcp)
add_test(NAME protocol_tests COMMAND protocol_tests)

if(BUILD_SERVER)
    add_executable(auth_policy_tests
        auth_policy_tests.cpp
    )
    target_link_libraries(auth_policy_tests PRIVATE server_auth)
    add_test(NAME auth_policy_tests COMMAND auth_policy_tests)
endif()
