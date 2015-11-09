FROM ubuntu:14.04
MAINTAINER zeliard <cyberjam@gmail.com>

RUN sudo apt-get update
RUN sudo apt-get install -y g++
RUN sudo apt-get install -y unzip
RUN sudo apt-get install -y wget
RUN sudo apt-get install -y build-essential
WORKDIR /tmp

RUN wget https://github.com/zeliard/EasyGameServer/archive/linux.zip
RUN unzip linux.zip
WORKDIR EasyGameServer-linux/EasyGameServer/EasyGameServerLinux/ 

RUN make
RUN cp -r database/ ./Debug/

WORKDIR Debug

CMD ["./EasyGameServerLinux"]

EXPOSE 9001

