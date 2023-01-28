

fpm \
  -s dir -t deb \
  -p libr5560-$VERSION-x64.deb \
  --name libr5560 \
  --license agpl3 \
  --version $VERSION \
  --architecture x64 \
  --depends bash --depends libzmq3-dev \
  --description "Low level SDK library for CAEN Nuclear Instruments x5560 digitizer family" \
  --url "https://github.com/NuclearInstruments/r5560_sdk" \
  --maintainer "Andrea Abba <abba@nuclearinstruments.eu>" \
  --chdir build_x64/output/



