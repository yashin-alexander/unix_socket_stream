BUILD_DIR=build

all:
	mkdir -p ${BUILD_DIR}
	cmake -S . -B ${BUILD_DIR}
	cmake --build ${BUILD_DIR}

clean:
	rm -rf ${BUILD_DIR}
	git clean -e ${BUILD_DIR}/* CMakeFiles CMakeCache.txt -f
