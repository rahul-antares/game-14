#!/usr/bin/env bash

pattern="$1"

input=$(cat)

if echo "$input" | grep -iq "$pattern"; then
  echo "$input"
fi

# usage
# ./test.js -b 10 | ./script.sh "grid printed"
