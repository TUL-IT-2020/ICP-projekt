#!/bin/bash
# By Pytel

apt_dependencies="apt-dependencies.txt"

# Update package list
sudo apt-get update

# Install apt dependencies
if [ -f $apt_dependencies ]; then
    while IFS= read -r package; do
        # Skip empty lines and comments
        if [[ -z "$package" || "$package" =~ ^# ]]; then
            continue
        fi
        echo "Installing $package"
        sudo apt-get install -y $package
        if [ $? -ne 0 ]; then
            echo "Error: Unable to locate package $package"
            exit 1
        fi
    done < $apt_dependencies
else
    echo "Error: $apt_dependencies file not found."
fi

# Install vcpkg if not already installed
if [ ! -d "vcpkg" ]; then
    git clone https://github.com/microsoft/vcpkg.git
    ./vcpkg/bootstrap-vcpkg.sh
fi

# Install nlohmann-json using vcpkg
./vcpkg/vcpkg install nlohmann-json

echo -e "Done!"
exit 0