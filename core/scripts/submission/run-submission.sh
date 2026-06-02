[ $# -eq 0 ] && {
    echo Usage: $0 VARIANT TARGET-DIR
    exit 1;
}

VARIANT=$1
DIR=$2

[ -z "$VARIANT" ] && { echo No VARIANT defined exit 1; }
[ -z "$DIR" ] && { echo No DIR defined exit 1; }

set -x

./index.js -b 10 -v $VARIANT -i 10000 > $DIR/v$VARIANT-regular.tsv
./index.js -b 15 -yf 1  -v $VARIANT -i 10000 > $DIR/v$VARIANT-enhanced.tsv
./index.js -b 25 -yf 2 -v $VARIANT -i 10000 > $DIR/v$VARIANT-super-boost.tsv
./index.js -b 55 -yf 3 -v $VARIANT -i 10000 > $DIR/v$VARIANT-wild-spin.tsv
./index.js -b 600 -yf 4 -v $VARIANT -i 10000 > $DIR/v$VARIANT-fb1.tsv
./index.js -b 1600 -yf 5 -v $VARIANT -i 10000 > $DIR/v$VARIANT-fb2.tsv
./index.js -b 5000 -yf 6 -v $VARIANT -i 10000 > $DIR/v$VARIANT-fb3.tsv
