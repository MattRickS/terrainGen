SHELL = /bin/bash

.PHONY: build
build:
	pushd build && cmake --build . && popd;

.PHONY: run
run:
	./build/app | csplit --quiet --elide-empty-files --prefix="images/${1:-img}" --suffix="%02d.ppm" - "/P3/" "{*}"

