SET(LIB_NAME octoon-ray)

SET(HEADER_PATH ${OCTOON_RAY_PATH_HEADER})
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
SOURCE_GROUP("octoon-ray\\Sequences" FILES ${SEQUENCES_LIST})

SET(BXDF_LIST
	${SOURCE_PATH}/BxDF.h
)
SOURCE_GROUP("octoon-ray\\BxDF" FILES ${BXDF_LIST})

SET(OCTOON_RAY_LIST
	${SOURCE_PATH}/main.cpp
	${SOURCE_PATH}/tiny_obj_loader.cpp
	${SOURCE_PATH}/tiny_obj_loader.h
	${SOURCE_PATH}/MonteCarlo.h
	${SOURCE_PATH}/MonteCarlo.cpp
	${SOURCE_PATH}/MonteCarloThread.h
	${SOURCE_PATH}/MonteCarloThread.cpp
)
SOURCE_GROUP("octoon-ray" FILES ${OCTOON_RAY_LIST})

ADD_EXECUTABLE(${LIB_NAME} ${OCTOON_RAY_LIST} ${SEQUENCES_LIST} ${BXDF_LIST})

TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC glfw)
TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC OpenGL32)
TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC RadeonRays)

SET_TARGET_ATTRIBUTE(${LIB_NAME} "octoon")