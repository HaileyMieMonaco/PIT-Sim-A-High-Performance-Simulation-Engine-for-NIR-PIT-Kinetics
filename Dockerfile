FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libopenblas-dev \
    libeigen3-dev \
    git

WORKDIR /app
COPY . .

RUN mkdir build && cd build && cmake .. && make
CMD ["./build/pit_sim"]