FROM ubuntu:16.10
RUN apt-get update
RUN apt-get install -y cmake
RUN apt-get install -y g++
RUN apt-get install -y libcurl4-gnutls-dev
RUN apt-get install -y libncurses-dev
RUN apt-get install -y libboost-dev
RUN apt-get install -y libboost-filesystem-dev
RUN apt-get install -y libboost-system-dev
RUN apt-get install -y libboost-program-options-dev
COPY CMakeLists.txt so_pro/
COPY src/ so_pro/src/
COPY urls.txt so_pro/run/
WORKDIR so_pro
RUN cmake .
RUN make
RUN mv so_pro run/
WORKDIR run
