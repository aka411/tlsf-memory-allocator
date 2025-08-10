#test cmake file

include(FetchContent)

FetchContent_declare(

	google-test
	GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2  #  specific version for stability

)



FetchContent_MakeAvailable(google-test)

include(GoogleTest)


add_subdirectory(unit)


