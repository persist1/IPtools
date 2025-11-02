#!/bin/bash
# Create release archives for all platforms
# Usage: ./create-release.sh <version>

set -e

VERSION=$1

if [ -z "$VERSION" ]; then
    echo "Usage: $0 <version>"
    echo "Example: $0 1.0.0"
    exit 1
fi

RELEASE_DIR="releases/v$VERSION"

echo "========================================"
echo "Creating release archives for v$VERSION"
echo "========================================"

# Create release directory
mkdir -p "$RELEASE_DIR"

# Function to create Windows release
create_windows_release() {
    local ARCH=$1
    local MINGW_ARCH=$2
    
    echo "Creating Windows $ARCH release..."
    
    if [ ! -f "release/IPtools.exe" ]; then
        echo "Error: release/IPtools.exe not found. Build the project first."
        return 1
    fi
    
    PACKAGE_NAME="IPtools-v$VERSION-Windows-$MINGW_ARCH"
    PACKAGE_DIR="$RELEASE_DIR/$PACKAGE_NAME"
    
    mkdir -p "$PACKAGE_DIR"
    cp release/IPtools.exe "$PACKAGE_DIR/"
    cp release/*.dll "$PACKAGE_DIR/" 2>/dev/null || true
    cp README.md "$PACKAGE_DIR/"
    cp USAGE_GUIDE.md "$PACKAGE_DIR/"
    cp CHANGELOG.md "$PACKAGE_DIR/"
    
    cd "$RELEASE_DIR"
    zip -r "$PACKAGE_NAME.zip" "$PACKAGE_NAME"
    rm -rf "$PACKAGE_NAME"
    cd - > /dev/null
    
    echo "Created: $RELEASE_DIR/$PACKAGE_NAME.zip"
}

# Function to create Linux release
create_linux_release() {
    local ARCH=$1
    
    echo "Creating Linux $ARCH release..."
    
    if [ ! -f "IPtools" ]; then
        echo "Error: IPtools binary not found. Build the project first."
        return 1
    fi
    
    PACKAGE_NAME="IPtools-v$VERSION-Linux-$ARCH"
    PACKAGE_DIR="$RELEASE_DIR/$PACKAGE_NAME"
    
    mkdir -p "$PACKAGE_DIR"
    cp IPtools "$PACKAGE_DIR/"
    cp README.md "$PACKAGE_DIR/"
    cp USAGE_GUIDE.md "$PACKAGE_DIR/"
    cp CHANGELOG.md "$PACKAGE_DIR/"
    
    cd "$RELEASE_DIR"
    tar -czf "$PACKAGE_NAME.tar.gz" "$PACKAGE_NAME"
    rm -rf "$PACKAGE_NAME"
    cd - > /dev/null
    
    echo "Created: $RELEASE_DIR/$PACKAGE_NAME.tar.gz"
}

# Function to create macOS release
create_macos_release() {
    local ARCH=$1
    
    echo "Creating macOS $ARCH release..."
    
    if [ -f "IPtools.dmg" ]; then
        PACKAGE_NAME="IPtools-v$VERSION-macOS-$ARCH.dmg"
        cp IPtools.dmg "$RELEASE_DIR/$PACKAGE_NAME"
        echo "Created: $RELEASE_DIR/$PACKAGE_NAME"
    elif [ -d "IPtools.app" ]; then
        PACKAGE_NAME="IPtools-v$VERSION-macOS-$ARCH"
        PACKAGE_DIR="$RELEASE_DIR/$PACKAGE_NAME"
        
        mkdir -p "$PACKAGE_DIR"
        cp -r IPtools.app "$PACKAGE_DIR/"
        cp README.md "$PACKAGE_DIR/"
        cp USAGE_GUIDE.md "$PACKAGE_DIR/"
        cp CHANGELOG.md "$PACKAGE_DIR/"
        
        cd "$RELEASE_DIR"
        tar -czf "$PACKAGE_NAME.tar.gz" "$PACKAGE_NAME"
        rm -rf "$PACKAGE_NAME"
        cd - > /dev/null
        
        echo "Created: $RELEASE_DIR/$PACKAGE_NAME.tar.gz"
    else
        echo "Error: IPtools.dmg or IPtools.app not found. Build the project first."
        return 1
    fi
}

# Detect platform and create appropriate release
case "$(uname -s)" in
    Linux*)
        ARCH=$(uname -m)
        create_linux_release "$ARCH"
        ;;
    Darwin*)
        ARCH=$(uname -m)
        create_macos_release "$ARCH"
        ;;
    CYGWIN*|MINGW*|MSYS*)
        # Detect Windows architecture
        if [ "$PROCESSOR_ARCHITECTURE" = "AMD64" ] || [ "$PROCESSOR_ARCHITEW6432" = "AMD64" ]; then
            create_windows_release "x64" "x86_64"
        else
            create_windows_release "x86" "i686"
        fi
        ;;
    *)
        echo "Unsupported platform: $(uname -s)"
        exit 1
        ;;
esac

echo ""
echo "========================================"
echo "Release creation completed!"
echo "Output directory: $RELEASE_DIR"
echo "========================================"
echo ""
echo "Next steps:"
echo "1. Test the release package"
echo "2. Create git tag: git tag -a v$VERSION -m 'Release v$VERSION'"
echo "3. Push tag: git push origin v$VERSION"
echo "4. GitHub Actions will automatically create the release"

