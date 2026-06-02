#!/usr/bin/env node

const fs = require('fs');

function printHelp() {
    console.log(`Usage: node check.js --max <maxValue> --bins <numBins> --file <filePath>
    Options:
        --max,  -m    Maximum value (integer)
        --bins, -b    Number of bins (integer)
        --file, -f    Path to the input file
    `);
    process.exit(1);
}

function parseArgs() {
    const args = process.argv.slice(2);
    let options = {};

    for (let i = 0; i < args.length; i++) {
        switch (args[i]) {
            case '--max': case '-m':
                options.maxValue = parseInt(args[++i], 10);
                break;
            case '--bins': case '-b':
                options.numBins = parseInt(args[++i], 10);
                break;
            case '--file': case '-f':
                options.filePath = args[++i];
                break;
            default:
                printHelp();
        }
    }

    if (!options.maxValue || !options.numBins || !options.filePath) {
        printHelp();
    }
    return options;
}

function createBins(maxValue, numBins) {
    const binSize = maxValue / numBins;
    let bins = [];
    for (let i = 0; i < numBins; i++) {
        bins.push({ range: [i * binSize, (i + 1) * binSize], count: 0 });
    }
    return bins;
}

function categorizeNumbers(filePath, maxValue, numBins) {
    const bins = createBins(maxValue, numBins);
    const numbers = fs.readFileSync(filePath, 'utf-8')
    .split('\n')
    .map(line => parseInt(line.trim(), 10))
    .filter(num => !isNaN(num));

    numbers.forEach(num => {
        for (let bin of bins) {
            if (num >= bin.range[0] && num < bin.range[1]) {
                bin.count++;
                break;
            }
        }
    });

    const totalNumbers = numbers.length;
    bins.forEach(bin => {
        bin.percentage = ((bin.count / totalNumbers) * 100).toFixed(4) + '%';
    });

    return { totalNumbers, bins };
}

function printBins(bins, totalNumbers, maxValue) {
    const expectedCount = Math.round(totalNumbers / bins.length);

    console.log(`Total Numbers = ${totalNumbers.toLocaleString()}\nBins = ${bins.length.toLocaleString()}\nMax-Value = ${maxValue.toLocaleString()}\n`);
    console.log("Bin Range   Count      Percentage   Expected   Deviation");
    console.log("------------------------------------------------------------");

    bins.forEach(bin => {
        const deviation = bin.count - expectedCount;
        const deviationPercentage = ((deviation / expectedCount) * 100).toFixed(4);

        console.log(
            `[${bin.range[0]}, ${bin.range[1]})`.padEnd(12) +
                `${bin.count.toLocaleString().padEnd(12)}` +
                `${bin.percentage.padEnd(12)}` +
                `${expectedCount.toLocaleString().padEnd(14)}` +
                `${deviation >= 0 ? '+' : ''}${deviation.toLocaleString()}(${deviationPercentage})%`
        );
    });
}

const options = parseArgs();
const { bins, totalNumbers } = categorizeNumbers(options.filePath, options.maxValue, options.numBins);
printBins(bins, totalNumbers, options.maxValue);
