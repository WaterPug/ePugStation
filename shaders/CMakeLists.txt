set(SHADER_FILES vertex.glsl fragment.glsl)

# Softlink in bin folder
foreach(shaderFile ${SHADER_FILES})
	configure_file(${shaderFile} ${CMAKE_CURRENT_BINARY_DIR}/${shaderFile} COPYONLY)
endforeach(shaderFile)