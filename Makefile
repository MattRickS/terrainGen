SHELL = /bin/bash

.PHONY: build
build:
	pushd build && cmake --build . && popd;

.PHONY: run
run:
	./build/app
