SET(LIB_NAME octoon-caustic)

SET(HEADER_PATH ${OCTOON_RAY_PATH_HEADER}/caustic)
SET(SOURCE_PATH ${OCTOON_RAY_PATH_SOURCE}/${LIB_NAME})

SET(SEQUENCES_LIST
	${SOURCE_PATH}/cranley_patterson.h
	${SOURCE_PATH}/cranley_patterson.cpp
	${SOURCE_PATH}/sequences.h
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
	${SOURCE_PATH}/BxDF.h
)
SOURCE_GROUP("octoon-caustic\\BxDF" FILES ${BXDF_LIST})

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
SOURCE_GROUP("octoon-caustic\\camera" FILES ${CAMERA_LIST})

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
SOURCE_GROUP("octoon-caustic\\light" FILES ${LIGHT_LIST})

SET(MATERIAL_LIST
	${HEADER_PATH}/material.h
)
SOURCE_GROUP("octoon-caustic\\material" FILES ${MATERIAL_LIST})

SET(SCENE_LIST
	${HEADER_PATH}/scene.h
)
SOURCE_GROUP("octoon-caustic\\scene" FILES ${SCENE_LIST})

SET(OCTOON_RAY_LIST
	${SOURCE_PATH}/main.cpp
	${SOURCE_PATH}/tiny_obj_loader.cpp
	${SOURCE_PATH}/tiny_obj_loader.h
	${SOURCE_PATH}/MonteCarlo.h
	${SOURCE_PATH}/MonteCarlo.cpp
	${SOURCE_PATH}/MonteCarloThread.h
	${SOURCE_PATH}/MonteCarloThread.cpp
)
SOURCE_GROUP("octoon-caustic" FILES ${OCTOON_RAY_LIST})

ADD_EXECUTABLE(${LIB_NAME} ${OCTOON_RAY_LIST} ${CAMERA_LIST} ${LIGHT_LIST} ${MATERIAL_LIST} ${SCENE_LIST} ${SEQUENCES_LIST} ${BXDF_LIST})

TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC glfw)
TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC OpenGL32)
TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC RadeonRays)

SET_TARGET_ATTRIBUTE(${LIB_NAME} "octoon")