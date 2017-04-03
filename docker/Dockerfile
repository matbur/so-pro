FROM ubuntu:16.10
RUN apt-get update > /dev/null 2>&1
RUN apt-get install -y cmake > /dev/null 2>&1
RUN apt-get install -y g++ > /dev/null 2>&1
RUN apt-get install -y libcurl4-gnutls-dev > /dev/null 2>&1
RUN apt-get install -y libncurses-dev > /dev/null 2>&1
RUN apt-get install -y libboost-dev > /dev/null 2>&1
RUN apt-get install -y libboost-system-dev > /dev/null 2>&1
RUN apt-get install -y libboost-filesystem-dev > /dev/null 2>&1
RUN apt-get install -y libboost-program-options-dev > /dev/null 2>&1
COPY CMakeLists.txt so_pro/
COPY src/ so_pro/src/
COPY urls.txt so_pro/run/
WORKDIR so_pro
RUN cmake .
RUN make -j
RUN mv so_pro run/
RUN echo "echo '\n'" >> ~/.bashrc
RUN echo "./so_pro" >> ~/.bashrc
RUN echo "echo '\nusage: ./so_pro -i FILE -n NUM'" >> ~/.bashrc
RUN echo "echo 'example: ./so_pro -i urls.txt -n 4\n'" >> ~/.bashrc
WORKDIR run
