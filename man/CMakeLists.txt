if(HELP2MAN_BINARY)
	add_custom_command(
		OUTPUT sailmcs.7
		DEPENDS sailmcs
		COMMAND ${HELP2MAN_BINARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sailmcs --include ${CMAKE_SOURCE_DIR}/man/sailmcs.7.h2m --no-info --section 7 -o sailmcs.7
	)

	add_custom_command(
		OUTPUT sailmcs.7.gz
		DEPENDS sailmcs.7
		COMMAND ${GZIP_BINARY} -k -f sailmcs.7
	)

	add_custom_target(
		manpage
		ALL
		DEPENDS sailmcs.7.gz
	)
else()
	message(WARNING "help2man is not installed. man page will not be built.")
endif()
