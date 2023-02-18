cd build_x64/output
fpm \
  -s dir -t deb \
  -p ../../libr5560-x64.deb \
  --name libr5560 \
  --license agpl3 \
  --version $CI_COMMIT_TAG \
  --architecture x86_64 \
  --description "Nuclear Instruments SciCompiler SDK" \
  --url "https://github.com/NuclearInstruments/r5560_sdk" \
  --maintainer "Andrea Abba <abba@nuclearinstruments.eu>" \
  .

cd ../..

cd build_i386/output
fpm \
  -s dir -t deb \
  -p ../../libr5560-i386.deb \
  --name libr5560 \
  --license agpl3 \
  --version $CI_COMMIT_TAG \
  --architecture all \
  --description "Nuclear Instruments SciCompiler SDK" \
  --url "https://github.com/NuclearInstruments/r5560_sdk" \
  --maintainer "Andrea Abba <abba@nuclearinstruments.eu>" \
  .
cd ../..

cd build_arm64/output
fpm \
  -s dir -t deb \
  -p ../../libr5560-arm64.deb \
  --name libr5560 \
  --license agpl3 \
  --version $CI_COMMIT_TAG \
  --architecture aarch64 \
  --description "Nuclear Instruments SciCompiler SDK" \
  --url "https://github.com/NuclearInstruments/r5560_sdk" \
  --maintainer "Andrea Abba <abba@nuclearinstruments.eu>" \
  .
cd ../..

cd build_armhf/output
fpm \
  -s dir -t deb \
  -p ../../libr5560-armhf.deb \
  --name libr5560 \
  --license agpl3 \
  --version $CI_COMMIT_TAG \
  --architecture armhf \
  --description "Nuclear Instruments SciCompiler SDK" \
  --url "https://github.com/NuclearInstruments/r5560_sdk" \
  --maintainer "Andrea Abba <abba@nuclearinstruments.eu>" \
  .
cd ../..
