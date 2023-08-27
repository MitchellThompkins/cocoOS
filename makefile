UID=$(shell id -u)
GID=$(shell id -g)

#######################################
### build #############################
#######################################

.PHONY: build.all
build.all: build.a9 build.x86_64

build.cmake/%/CMakeCache.txt:
	cmake --preset $*

build.cmake.%: | build.cmake/%/CMakeCache.txt
	cmake --build --parallel ${nproc} --preset $*

build.%: build.cmake.%
	echo "build $*"

build.graph:
	cmake --graphviz=test.dot . -DPLATFORM=CORTEX_A9


#######################################
### test #########################
#######################################
.PHONY: test
test: 
	python3 scripts/test.py -t tests/tests.json 


#######################################
### container #########################
#######################################

.PHONY: container.pull
container.pull:
	docker pull ghcr.io/mitchellthompkins/embedded_sdk:latest

.PHONY: container.start
container.start:
	docker-compose -f docker-compose.yml run --rm dev_env 'sh -x'


#######################################
### utility ###########################
#######################################

#TODO(@mthompkins): Use poetry to manage deps
.PHONY: check-trace
check-trace:
	python3 -m pip install click termcolor 
	python3 scripts/trace_reqs.py \
		--req documents/requirements.csv \
		--test \
			cpputest_TestOsTask.xml \
			cpputest_TestOsKernel.xml
	
.PHONY: clean
clean:
	rm -rf build/

#######################################
### debug ###########################
#######################################
include debug.mk


# All the Makefiles read themselves get matched if a target exists for them, so
# they will get matched by a Match anything target %:. This target is here
# to prevent the %: Match-anything target from matching, and do nothing.
Makefile:
	;
