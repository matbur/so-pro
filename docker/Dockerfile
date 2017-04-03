FROM ubuntu:16.10
RUN bash -c "apt-get update &> /dev/null"
RUN bash -c "apt-get install -y cmake &> /dev/null"
RUN bash -c "apt-get install -y g++ &> /dev/null"
RUN bash -c "apt-get install -y libcurl4-gnutls-dev &> /dev/null"
RUN bash -c "apt-get install -y libncurses-dev &> /dev/null"
RUN bash -c "apt-get install -y libboost-dev &> /dev/null"
RUN bash -c "apt-get install -y libboost-filesystem-dev &> /dev/null"
RUN bash -c "apt-get install -y libboost-system-dev &> /dev/null"
RUN bash -c "apt-get install -y libboost-program-options-dev &> /dev/null"
COPY CMakeLists.txt so_pro/
COPY src/ so_pro/src/
COPY urls.txt so_pro/run/
WORKDIR so_pro
RUN cmake .
RUN make
RUN mv so_pro run/
RUN echo "echo '\n'" >> ~/.bashrc
RUN echo "./so_pro" >> ~/.bashrc
RUN echo "echo '\nusage: ./so_pro -i FILE -n NUM\n'" >> ~/.bashrc
RUN echo "echo '\nexample: ./so_pro -i urls.txt -n 4\n'" >> ~/.bashrc
WORKDIR run
