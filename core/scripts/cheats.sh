#!/bin/bash
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
GRAY='\033[0;90m'
BLINK='\033[5m'
DIM='\033[2m'
DIM_GRAY='\033[2;90m'
NC='\033[0m'
TICK="✓"
CROSS="✗"
SUCCESS_COUNT=0
ERROR_COUNT=0
TOTAL_COUNT=0
BASE_SCRIPT="./test.js -b 10 -g c -e n"

if [ $# -eq 0 ]; then
    echo -e "${RED}Error: Please provide the cheats file as an argument${NC}"
    echo "Usage: $0 <cheats-file>"
    exit 1
fi

CHEATS_FILE="$1"

if [ ! -f "$CHEATS_FILE" ]; then
    echo -e "${RED}Error: File '$CHEATS_FILE' not found${NC}"
    exit 1
fi

TOTAL_TESTS=$(grep -c . "$CHEATS_FILE")

# Read file into array to avoid stdin conflicts
mapfile -t CHEATS < "$CHEATS_FILE"

for cheat in "${CHEATS[@]}"; do
    # strip carriage returns
    cheat=$(echo "$cheat" | tr -d '\r')

    # skip empty lines
    if [ -z "$cheat" ]; then
        continue
    fi

    CURRENT_TEST=$((TOTAL_COUNT + 1))
    FULL_COMMAND="$BASE_SCRIPT $cheat"

    echo -ne "${DIM_GRAY}[ ${NC}${GREEN}${SUCCESS_COUNT}${TICK}${NC}, ${RED}${ERROR_COUNT}${CROSS}${NC}${GRAY} ]${NC} ${DIM_GRAY}(${NC}${CURRENT_TEST}/${TOTAL_TESTS}${BLINK}${YELLOW}...${NC}${DIM_GRAY})${NC} ${FULL_COMMAND}"

    START_TIME=$(date +%s.%N)
    OUTPUT=$($FULL_COMMAND 2>&1)
    EXIT_STATUS=$?
    END_TIME=$(date +%s.%N)
    DURATION=$(echo "$END_TIME - $START_TIME" | bc)

    # clear "executing..." line to reprint with result
    echo -ne "\r\033[K"

    # succeeded or failed
    if [ $EXIT_STATUS -eq 0 ]; then
        ((SUCCESS_COUNT++))
        echo -e "${DIM_GRAY}[ ${NC}${GREEN}${SUCCESS_COUNT}${TICK}${NC}, ${RED}${ERROR_COUNT}${CROSS}${NC}${GRAY} ]${NC} ${GREEN}(${CURRENT_TEST}/${TOTAL_TESTS}...${TICK})${NC} ${FULL_COMMAND}"
    else
        ((ERROR_COUNT++))
        echo -e "${DIM_GRAY}[ ${NC}${GREEN}${SUCCESS_COUNT}${TICK}${NC}, ${RED}${ERROR_COUNT}${CROSS}${NC}${GRAY} ]${NC} ${RED}(${CURRENT_TEST}/${TOTAL_TESTS}...${CROSS})${NC} ${FULL_COMMAND}"
        echo -e "${DIM_GRAY}${OUTPUT}${NC}"
    fi

    ((TOTAL_COUNT++))
done

echo ""
echo -e "${CYAN}Final Results:${NC}"
echo -e "${GREEN}Successful: ${SUCCESS_COUNT} ${TICK}${NC}"
echo -e "${RED}Failed: ${ERROR_COUNT} ${CROSS}${NC}"
echo -e "${BLUE}Total: ${TOTAL_COUNT}${NC}"
