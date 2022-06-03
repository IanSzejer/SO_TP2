#!/bin/bash
cd Toolchain
docker run -v ${PWD}:/root -u $(id -u ${USER}):$(id -g ${USER}) --security-opt seccomp:unconfined -ti -w /root agodio/itba-so:1.0 ${1} make all
cd ..
docker run -v ${PWD}:/root -u $(id -u ${USER}):$(id -g ${USER}) --security-opt seccomp:unconfined -ti -w /root agodio/itba-so:1.0 ${1} make all 