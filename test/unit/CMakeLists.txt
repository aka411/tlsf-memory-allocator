#unit test cmake file


add_executable(tlsf-allocator-test 
     sanity_check.cpp
	#tlsf_allocator_test.cpp
)



target_link_libraries(tlsf-allocator-test PRIVATE
	#tlsf-allocator
    GTest::gtest_main
)

gtest_discover_tests(tlsf-allocator-test)