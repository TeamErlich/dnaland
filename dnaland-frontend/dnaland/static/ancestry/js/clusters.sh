#!/bin/bash

# This reads a file from the ancestry git repo and creates a js-based version
# of it here.  Since it's a separate repo, we need the path.

if [ "$#" != 1 ]; then 
    echo "This script requires one parameter, the path of the ancestry repo"
    echo "   (often ~/ancestry)"
    exit 1
fi

echo "var Clusters = ["
cat ${1}/testing/clusters  | sed 's/ /", "/g' | sed 's/^/  ["/' | sed 's/$/"],/'
echo "];"

echo "/*************************************************"
echo "This is an auto-generated file."
echo "Rerun $0 to regenerate it"
echo -n "*************************************************"
echo "/"
