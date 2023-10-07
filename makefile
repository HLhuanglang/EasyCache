#define some variables

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR =${ROOT_DIR}/build

export BUILD_DIR


#compile rules

.PHONY:default
default:
	echo "make [inprocess_cache|cache]"

.PHONY:premake
premake:
	if [ ! -d "${BUILD_DIR}" ]; then mkdir ${BUILD_DIR}; fi

.PHONY:inprocess_cache
inprocess_cache: premake
	make -C inprocess_cache