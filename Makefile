BUILD_DIR=build
SYSTEMD_DIR=/etc/systemd/user
SYSTEMD_SERVICE_NAME=sock-stream.service 
APP_INSTALLATION_DIR=/tmp/socket-stream

all:
	mkdir -p ${BUILD_DIR}
	cmake -S . -B ${BUILD_DIR}
	cmake --build ${BUILD_DIR}

install:
	cmake --install ${BUILD_DIR} --prefix /tmp/socket-stream
	cp tools/${SYSTEMD_SERVICE_NAME} ${SYSTEMD_DIR}

uninstall:
	rm -rf ${SYSTEMD_DIR}/${SYSTEMD_SERVICE_NAME}
	rm -rf ${APP_INSTALLATION_DIR}

clean:
	rm -rf ${BUILD_DIR}

