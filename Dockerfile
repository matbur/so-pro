FROM ubuntu:16.10
RUN echo " \
    \necho \
    \n./so_pro \
    \necho ' \
        \nusage: ./so_pro -i FILE -n NUM \
        \nexample: ./so_pro -i urls.txt -n 4\n'" \
    >> ~/.bashrc
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    libboost-dev \
    libboost-filesystem-dev \
    libboost-program-options-dev \
    libboost-system-dev \
    libcurl4-gnutls-dev \
    libncurses-dev
COPY CMakeLists.txt so_pro/ 
COPY src/ so_pro/src/
COPY urls.txt so_pro/run/
WORKDIR so_pro
RUN cmake . && make -j && mv so_pro run/
WORKDIR run
