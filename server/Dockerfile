FROM ubuntu:16.04

RUN apt-get update

RUn apt-get install -y sudo

RUN sudo apt-get update

RUN sudo apt install -y git-all

RUN sudo apt-get install wget 

RUN sudo apt-get install -y libmicrohttpd-dev libjansson-dev libcurl4-gnutls-dev libgnutls28-dev libgcrypt20-dev

RUN sudo apt-get install -y cmake

RUN git clone https://github.com/babelouest/ulfius.git && cd ulfius/ && git submodule update --init && cd lib/orcania && make && sudo make install && cd ../yder && make && sudo make install && cd ../.. && make && sudo make install

RUN wget http://www.digip.org/jansson/releases/jansson-2.11.tar.bz2 && bunzip2 -c jansson-2.11.tar.bz2 | tar xf -

RUN cd jansson-2.11 && ./configure && make && make check && make install

COPY ./server.c /server.c
COPY ./Makefile /Makefile

# Expose the port that the server will listen on
EXPOSE 1234

CMD ["make"]
