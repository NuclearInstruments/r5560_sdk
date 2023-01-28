
#!/bin/bash

VERSION="1.2.0"

export V_MAJ=$(echo $VERSION | cut -d'.' -f1)
export V_MIN=$(echo $VERSION | cut -d'.' -f2)
export V_MIC=$(echo $VERSION | cut -d'.' -f3)

echo "Major version: $V_MAJ"
echo "Minor version: $V_MIN"
echo "Micro version: $V_MIC"

autoreconf -if


#compile x64 (native)
rm -rf build_x64
mkdir -p build_x64
cd build_x64
../configure --prefix=$(pwd)/output
make -j16 && make install
cd ..

