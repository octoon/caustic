SET(LIB_NAME octoon-caustic)

SET(HEADER_PATH ${OCTOON_PATH_HEADER}/caustic)
SET(SOURCE_PATH ${OCTOON_PATH_SOURCE}/${LIB_NAME})

SET(MATH_LIST
	${HEADER_PATH}/math.h
)
SOURCE_GROUP("octoon-caustic\\math" FILES ${MATH_LIST})

SET(SEQUENCES_LIST
	${SOURCE_PATH}/cranley_patterson.h
	${SOURCE_PATH}/cranley_patterson.cpp
	${HEADER_PATH}/sequences.h
	${SOURCE_PATH}/sequences.cpp
	${SOURCE_PATH}/hammersley.h
	${SOURCE_PATH}/hammersley.cpp
	${SOURCE_PATH}/halton.h
	${SOURCE_PATH}/halton.cpp
	${SOURCE_PATH}/halton_enum.h
	${SOURCE_PATH}/halton_sampler.h
)
SOURCE_GROUP("octoon-caustic\\sequences" FILES ${SEQUENCES_LIST})

SET(BXDF_LIST
	${SOURCE_PATH}/disney.h
	${SOURCE_PATH}/disney.cpp
)
SOURCE_GROUP("octoon-caustic\\BRDF" FILES ${BXDF_LIST})

SET(MATERIAL_LIST
	${HEADER_PATH}/material.h
)
SOURCE_GROUP("octoon-caustic\\material" FILES ${MATERIAL_LIST})

SET(SCENE_LIST
	${HEADER_PATH}/render_object.h
	${SOURCE_PATH}/render_object.cpp
	${HEADER_PATH}/render_scene.h
	${SOURCE_PATH}/render_scene.cpp
)
SOURCE_GROUP("octoon-caustic\\scene" FILES ${SCENE_LIST})

SET(CAMERA_LIST
	${HEADER_PATH}/camera.h
	${SOURCE_PATH}/camera.cpp
	${HEADER_PATH}/ortho_camera.h
	${SOURCE_PATH}/ortho_camera.cpp
	${HEADER_PATH}/film_camera.h
	${SOURCE_PATH}/film_camera.cpp
	${HEADER_PATH}/environment_camera.h
	${SOURCE_PATH}/environment_camera.cpp
)
SOURCE_GROUP("octoon-caustic\\scene\\camera" FILES ${CAMERA_LIST})

SET(LIGHT_LIST
	${HEADER_PATH}/light.h
	${SOURCE_PATH}/light.cpp
	${HEADER_PATH}/directional_light.h
	${SOURCE_PATH}/directional_light.cpp
	${HEADER_PATH}/point_light.h
	${SOURCE_PATH}/point_light.cpp
	${HEADER_PATH}/spot_light.h
	${SOURCE_PATH}/spot_light.cpp
	${HEADER_PATH}/sphere_light.h
	${SOURCE_PATH}/sphere_light.cpp
)
SOURCE_GROUP("octoon-caustic\\scene\\light" FILES ${LIGHT_LIST})

SET(GEOMETRY_LIST
	${HEADER_PATH}/geometry.h
	${SOURCE_PATH}/geometry.cpp
)
SOURCE_GROUP("octoon-caustic\\scene\\geometry" FILES ${GEOMETRY_LIST})

SET(SAMPLER_LIST
	${HEADER_PATH}/sampler.h
)
SOURCE_GROUP("octoon-caustic\\sampler" FILES ${SAMPLER_LIST})

SET(TEXTURE_LIST
	${HEADER_PATH}/texture.h
)
SOURCE_GROUP("octoon-caustic\\texture" FILES ${TEXTURE_LIST})

SET(PIPELINE_LIST
	${SOURCE_PATH}/montecarlo.h
	${SOURCE_PATH}/montecarlo.cpp
	${HEADER_PATH}/pipeline.h
	${SOURCE_PATH}/pipeline.cpp
)
SOURCE_GROUP("octoon-caustic\\pipeline" FILES ${PIPELINE_LIST})

SET(SPECTRUM_LIST
	${HEADER_PATH}/spectrum.h
	${SOURCE_PATH}/spectrum.cpp
	${HEADER_PATH}/rgb_spectrum.h
	${SOURCE_PATH}/rgb_spectrum.cpp
)
SOURCE_GROUP("octoon-caustic\\spectrum" FILES ${SPECTRUM_LIST})

SET(TONEMAPPING_LIST
	${HEADER_PATH}/tonemapping.h
	${SOURCE_PATH}/tonemapping.cpp
	${HEADER_PATH}/ACES.h
	${SOURCE_PATH}/ACES.cpp
)
SOURCE_GROUP("octoon-caustic\\tonemapping" FILES ${TONEMAPPING_LIST})

SET(SYSTEM_LIST
	${HEADER_PATH}/system.h
	${SOURCE_PATH}/system.cpp
	${SOURCE_PATH}/main.cpp
	${SOURCE_PATH}/tiny_obj_loader.cpp
	${SOURCE_PATH}/tiny_obj_loader.h
)
SOURCE_GROUP("octoon-caustic" FILES ${SYSTEM_LIST})

ADD_EXECUTABLE(${LIB_NAME} 
	${MATH_LIST}
	${CAMERA_LIST} 
	${LIGHT_LIST}
	${GEOMETRY_LIST}
	${MATERIAL_LIST}
	${SCENE_LIST}
	${SEQUENCES_LIST}
	${SAMPLER_LIST}
	${TEXTURE_LIST}
	${BXDF_LIST}
	${PIPELINE_LIST}
	${TONEMAPPING_LIST}
	${SPECTRUM_LIST}
	${SYSTEM_LIST} 
)

TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC glfw)
TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC OpenGL32)
TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC RadeonRays)

SET_TARGET_ATTRIBUTE(${LIB_NAME} "octoon")