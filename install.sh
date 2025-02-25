#!/bin/bash
# By Pytel

apt_dependencies="apt-dependencies.txt"

# Update package list
sudo apt-get update

# Install apt dependencies
if [ -f $apt_dependencies ]; then
    while IFS= read -r package; do
        if ! sudo apt-get install -y $package; then
            echo "Error: Unable to locate package $package"
        fi
    done < $apt_dependencies
else
    echo "Error: $apt_dependencies file not found."
fi

echo -e "Done!"