# Multi-stage Dockerfile for IPtools
# Usage: docker build -t iptools:latest .
#        docker run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix iptools:latest

ARG QT_VERSION=5.15.2
ARG UBUNTU_VERSION=20.04

# Build stage
FROM ubuntu:${UBUNTU_VERSION} as builder

ENV DEBIAN_FRONTEND=noninteractive
ENV QT_VERSION=${QT_VERSION}

# Install build dependencies
RUN apt-get update && apt-get install -y \
    qt5-default \
    qtbase5-dev \
    qttools5-dev-tools \
    build-essential \
    lsof \
    libgl1-mesa-dev \
    libglib2.0-0 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

# Copy source code
COPY *.cpp *.h *.ui *.pro ./
COPY *.md ./

# Build the project
RUN qmake IPtools.pro CONFIG+=release && \
    make -j$(nproc) && \
    strip IPtools

# Runtime stage
FROM ubuntu:${UBUNTU_VERSION}

ENV DEBIAN_FRONTEND=noninteractive

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libqt5core5a \
    libqt5gui5 \
    libqt5widgets5 \
    libqt5network5 \
    lsof \
    libgl1 \
    libglib2.0-0 \
    && rm -rf /var/lib/apt/lists/*

# Copy binary from builder
COPY --from=builder /build/IPtools /usr/local/bin/
COPY README.md USAGE_GUIDE.md /usr/share/doc/iptools/

# Create a non-root user (optional, for non-privileged operations)
RUN useradd -m -s /bin/bash iptools

# Note: For process/file operations, you'll likely need to run as root
# WORKDIR /home/iptools
# USER iptools

# Default command
ENTRYPOINT ["/usr/local/bin/IPtools"]

# Metadata
LABEL maintainer="Your Name"
LABEL description="IPtools - IP and System Utilities"
LABEL version="1.0.0"

