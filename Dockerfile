# Base image
FROM ubuntu:24.04
LABEL authors="mau_vv"

# Install build tools
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    cmake \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Build serial and parallel versions
RUN g++ -std=c++17 Sokoban-Serial/Source.cpp -o serial_app
RUN g++ -std=c++17 -fopenmp Sokoban-Datos/Source.cpp -o parallel_app

# Default command: start a bash shell
CMD ["/bin/bash"]
