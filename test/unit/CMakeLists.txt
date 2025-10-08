#unit test cmake file


add_executable(tlsf-allocator-test 
     sanity_check.cpp
	 tlsf_allocator_public_test.cpp
	 tlsf_allocator_internal_test.cpp
)



target_link_libraries(tlsf-allocator-test PRIVATE
	tlsf::tlsf-allocator
    GTest::gtest_main
)

gtest_discover_tests(tlsf-allocator-test)