[ $# -eq 0 ] && {
    echo Usage: $0 VARIANT TARGET-DIR
    exit 1;
}

VARIANT=$1
DIR=$2

[ -z "$VARIANT" ] && { echo No VARIANT defined exit 1; }
[ -z "$DIR" ] && { echo No DIR defined exit 1; }

set -x

./index.js -G 0 -b 10 -v $VARIANT -i 1000000 > $DIR/v$VARIANT-regular.tsv
./index.js -G 0 -b 15 -yf 1  -v $VARIANT -i 1000000 > $DIR/v$VARIANT-enhanced.tsv
./index.js -G 0 -b 550 -yf 2 -v $VARIANT -i 1000000 > $DIR/v$VARIANT-fb1.tsv
./index.js -G 0 -b 1050 -yf 3 -v $VARIANT -i 1000000 > $DIR/v$VARIANT-fb2.tsv
./index.js -G 0 -b 1950 -yf 4 -v $VARIANT -i 1000000 > $DIR/v$VARIANT-fb3.tsv
./index.js -G 0 -b 3450 -yf 5 -v $VARIANT -i 1000000 > $DIR/v$VARIANT-fb4.tsv
./index.js -G 0 -b 5850 -yf 6 -v $VARIANT -i 1000000 > $DIR/v$VARIANT-fb5.tsv
