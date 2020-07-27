FROM --platform=linux/amd64 adoptopenjdk:11-jdk-hotspot

RUN apt-get update && apt-get install -y build-essential wget && apt-get clean

RUN wget -q https://github.com/Kitware/CMake/releases/download/v3.15.2/cmake-3.15.2-Linux-x86_64.sh && \
  chmod +x cmake-3.15.2-Linux-x86_64.sh && \
  ./cmake-3.15.2-Linux-x86_64.sh  --skip-license && \
  rm cmake-3.15.2-Linux-x86_64.sh

CMD /bin/bash
