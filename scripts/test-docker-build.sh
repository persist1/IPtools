#!/bin/bash
# Test Docker multi-architecture build locally
# Usage: ./test-docker-build.sh [amd64|arm64|armv7|all]

set -e

ARCH=${1:-amd64}

echo "========================================"
echo "Testing Docker build for $ARCH"
echo "========================================"

# Check if Docker is installed
if ! command -v docker &> /dev/null; then
    echo "Error: Docker is not installed"
    exit 1
fi

# Check if buildx is available
if ! docker buildx version &> /dev/null; then
    echo "Error: Docker buildx is not available"
    echo "Please update Docker to a version that supports buildx"
    exit 1
fi

# Create buildx builder if it doesn't exist
if ! docker buildx inspect multiarch-builder &> /dev/null; then
    echo "Creating multiarch builder..."
    docker buildx create --name multiarch-builder --use
fi

build_architecture() {
    local PLATFORM=$1
    local TAG=$2
    
    echo ""
    echo "Building for $PLATFORM..."
    docker buildx build \
        --platform "$PLATFORM" \
        --tag "iptools:$TAG" \
        --load \
        .
    
    echo "Build successful for $PLATFORM"
    echo "Image: iptools:$TAG"
}

case "$ARCH" in
    amd64)
        build_architecture "linux/amd64" "amd64"
        ;;
    arm64)
        build_architecture "linux/arm64" "arm64"
        ;;
    armv7)
        build_architecture "linux/arm/v7" "armv7"
        ;;
    all)
        build_architecture "linux/amd64" "amd64"
        build_architecture "linux/arm64" "arm64"
        build_architecture "linux/arm/v7" "armv7"
        ;;
    *)
        echo "Error: Invalid architecture '$ARCH'"
        echo "Usage: $0 [amd64|arm64|armv7|all]"
        exit 1
        ;;
esac

echo ""
echo "========================================"
echo "Docker build test completed!"
echo "========================================"
echo ""
echo "To run the container:"
echo "  docker run --rm -it iptools:$ARCH"
echo ""
echo "To extract the binary:"
echo "  docker create --name temp iptools:$ARCH"
echo "  docker cp temp:/usr/local/bin/IPtools ./IPtools-$ARCH"
echo "  docker rm temp"

