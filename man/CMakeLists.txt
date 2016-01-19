if(HELP2MAN_BINARY)
	add_custom_command(
		OUTPUT sailmcs.7
		MAIN_DEPENDENCY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sailmcs
		COMMAND ${HELP2MAN_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sailmcs --include ${CMAKE_SOURCE_DIR}/man/sailmcs.7.h2m --no-info --section 7 -o sailmcs.7
	)

	add_custom_command(
		OUTPUT sailmcs.7.gz
		MAIN_DEPENDENCY sailmcs.7
		COMMAND ${GZIP_BINARY} -k sailmcs.7
	)

	add_custom_target(
		manpage
		ALL
		DEPENDS sailmcs.7.gz
	)
else()
	message(WARNING "help2man is not installed. man page will not be built.")
endif()
