const Big                  = require('big.js');
const moment               = require('moment');
const momentDurFormatSetup = require('moment-duration-format');
const fs                   = require('fs');
const colors               = require('colors');
const { exit } = require('process');
const sprintf              = require('sprintf-js').sprintf;

function round(num, decimalPlaces = 0) {
    const p = Math.pow(10, decimalPlaces);
    const n = (num * p) * (1 + Number.EPSILON);
    return Math.round(n) / p;
}

class Data {
    static iterCount    = 0;
    static iterations   = 0;
    static totalWins    = 0;
    static baseCoinWins = 0;
    static baseLineWins = 0;
    static respinWins   = 0;
    static freespinWins = 0;
    static fsCoinWins   = 0;
    static fsLineWins   = 0;
    static fsRespinWins = 0;

    static rtp          = 0;
    static baseCoinRTP  = 0;
    static baseLineRTP  = 0;
    static respinRTP    = 0;
    static freespinRTP  = 0;
    static fsCoinRTP    = 0;
    static fsLineRTP    = 0;
    static fsRespinRTP  = 0;
    static respinCount  = 0;
    static freespinCount  = 0;
    static respinWinCount = 0;
    static freespinWinCount = 0;
    static respinOccCount = 0;

    static totalBets    = 0;
    static winCount     = 0;
    static sbCount      = 0;
    static reelSets     = {};
    static fsCounts     = [];
    static maxWin       = 0;
    static maxWinCount  = 0;
    static fsTotalCount = 0;
    static baseSpinWins = 0;
    static baseSpinRtp  = 0;
    static winMap       = {};
    static startTS      = new Date();

    // NOTE: might add a gamble occurence count

    static mergeWinMap (winMap) {
        for (let xWin in winMap) {
            if (!Data.winMap[xWin])
                Data.winMap[xWin] = 0;

            Data.winMap[xWin] += winMap[xWin];
        }
    }

    static Mean () {
        let total = Big(0), totalCount = Big(0);

        for (let xWin in Data.winMap) {
            const multi = Big(xWin);
            const count = Big(Data.winMap[xWin]);

            total = multi.times(count).add(total);
            totalCount = totalCount.add(count);
        }

        Data.mean = Big(0);

        if (!totalCount.eq(0))
            Data.mean = total.div(totalCount);
    }

    static StandardDev () {
        let v = Big(0);

        for (let xWin in Data.winMap) {
            const multi = Big(xWin);
            const count = Big(Data.winMap[xWin]);

            const a = multi.minus(Data.mean).pow(2).times(count);
            v = v.add(a);
        }

        Data.variance = v.div(Data.iterations);
        Data.stdDev   = Data.variance.sqrt();
    }

    static ConfidenceInterval () {
        const meanVariance = Data.variance.div(Data.iterations);
        Data.standardError = meanVariance.sqrt();
        Data.marginError   = Data.standardError.times(1.960);
        Data.minCI         = Data.mean.minus(Data.marginError).times(100);
        Data.maxCI         = Data.mean.plus(Data.marginError).times(100);
        const withinCI     = Data.rtp.gte(Data.minCI) && Data.rtp.lte(Data.maxCI);
        Data.rtpColor      = withinCI ? colors.yellow:colors.red;
    }

    static accumulate (data) {
        Data.iterCount        += 1;
        Data.iterations       += data.batch.iterations;
        Data.totalWins        += data.batch.totalWins;
        Data.totalBets        += data.batch.totalBets;
        Data.respinWins       += data.batch.respinWins;
        Data.freespinWins     += data.batch.freespinWins;
        Data.fsCoinWins       += data.batch.fsCoinWins;
        Data.fsLineWins       += data.batch.fsLineWins;
        Data.fsRespinWins     += data.batch.fsRespinWins;
        Data.baseCoinWins     += data.batch.baseCoinWins;
        Data.baseLineWins     += data.batch.baseLineWins;
        Data.winCount         += data.batch.winCount;
        Data.maxWinCount      += data.batch.maxWinCount;
        Data.baseSpinWins     += data.batch.baseSpin.totalWins;
        Data.sbCount          += data.batch.sbCount;
        Data.fsTotalCount     += data.batch.fsTotalCount;
        Data.respinCount      += data.batch.respinCount;
        Data.freespinCount    += data.batch.freespinCount;
        Data.respinWinCount   += data.batch.respinWinCount;
        Data.freespinWinCount += data.batch.freespinWinCount;
        Data.respinOccCount   += data.batch.respinOccCount;

        Data.accumFsCounts(data.batch.fsCounts);
        Data.accumReelSets(data.batch.reelSets);
        Data.mergeWinMap (data.batch.winMap);

        Data.rtp         = Big(100).times(Data.totalWins).div(Data.totalBets);
        Data.baseLineRTP = Big(100).times(Data.baseLineWins).div(Data.totalBets);
        Data.baseCoinRTP = Big(100).times(Data.baseCoinWins).div(Data.totalBets);
        Data.baseSpinRtp = Big(100).times(Data.baseSpinWins).div(Data.totalBets);
        Data.respinRTP   = Big(100).times(Data.respinWins).div(Data.totalBets);
        Data.freespinRTP = Big(100).times(Data.freespinWins).div(Data.totalBets);
        Data.fsLineRTP   = Big(100).times(Data.fsLineWins).div(Data.totalBets);
        Data.fsCoinRTP   = Big(100).times(Data.fsCoinWins).div(Data.totalBets);
        Data.fsRespinRTP = Big(100).times(Data.fsRespinWins).div(Data.totalBets);
        Data.hitRate     = Big(100).times(Data.winCount).div(Data.iterations);

        Data.Mean ();
        Data.StandardDev ();
        Data.ConfidenceInterval ();
    }

    static accumReelSets (reelSet) {
        if ((Object.keys(Data.reelSets)).length === 0) {
            Data.reelSets = reelSet;
            return;
        }

        Object.keys(reelSet).forEach((reelSetItem) => {
            Data.reelSets[reelSetItem] += reelSet[reelSetItem];
        });
    }

    static accumFsCounts (fsCounts) {
        if (Data.fsCounts.length === 0) {
            Data.fsCounts = fsCounts;
            return;
        }

        for (let i=0; i<fsCounts.length; i++) {
            Data.fsCounts[i] += fsCounts[i];
        }
    }

    // TODO: this should be updated
    static showCsvOutput(hData) {
        const csvOutKeys = [
            'iterations', 'mean',
            'rtp', 'baseSpinRtp',
            'hitRate', 'fsHitRate',
            'fsTotalCount', 'sbCount',
            'maxWin(batch)', 'totalWin(batch)',
            'totalBets(batch)', 'winCount', 'baseSpinWins',
            'variance', 'stdDeviation',
        ];
        const csvHeader = csvOutKeys.join(',');
        if (Data.iterations === 0)
            console.log (csvHeader);

        const { batch } = hData;
        Data.accumulate (hData);

        const hitMaxWin = Data.iterations/Data.maxWinCount;
        const hitFS     = Data.iterations/Data.fsTotalCount;
        console.log (`${Data.iterations},${Data.mean},${Data.rtp}, ${Data.baseSpinRtp},${hitMaxWin},${hitFS},${Data.fsTotalCount},${Data.sbCount},${batch.maxWin},${batch.totalWins},${batch.totalBets},${Data.winCount},${Data.baseSpinWins},${Data.variance},${Data.stdDev}`);
    }

    static showAccStats (hData) {
        const { batch } = hData;
        const duration = new moment.duration (batch.duration, 'ms');
        const indicator = "Σ";
        Data.accumulate (hData);

        console.log (
            colors.green (`${sprintf ("+%12s", Data.iterations.toLocaleString())} [${duration.format ('hh:mm:ss.SSS', { trim:false })}]`),
            `: ${indicator}(RTP=${colors.yellow (sprintf ("%6.3f%%", round(Data.rtp, 3)))}`,
            `hit=${colors.yellow(sprintf("%4.3f", round(Data.winCount* 100/Data.iterations, 3)))})`,
            `max-win=${colors.yellow(sprintf("%.2f", batch.maxWin))}`,
            `w/b=${batch.totalWins.toLocaleString()}/${batch.totalBets.toLocaleString()}`,
        );

        console.log (`      ${indicator}(RTP`,
            `Base(Lines=${colors.magenta (sprintf ("%6.3f%%", round(Data.baseLineRTP, 3)))},`,
            `Coins=${colors.magenta (sprintf ("%6.3f%%", round(Data.baseCoinRTP, 3)))})`,
            `Re-Spin=${colors.magenta (sprintf ("%6.3f%%", round(Data.respinRTP, 3)))}`,
            `Free-Spin=${colors.magenta (sprintf ("%6.3f%%", round(Data.freespinRTP, 3)))} )`
        );

        console.log (`      ${indicator}(RTP`,
            `Free Spin (Lines=${colors.magenta (sprintf ("%6.3f%%", round(Data.fsLineRTP, 3)))},`,
            `Coins=${colors.magenta (sprintf ("%6.3f%%", round(Data.fsCoinRTP, 3)))})`,
            `Re-Spin=${colors.magenta (sprintf ("%6.3f%%", round(Data.fsRespinRTP, 3)))}`,
        );

        console.log (`      ${indicator} ` +
            `mean ${colors.yellow(round(Data.mean.toLocaleString(), 4))}, ` +
            `σ\u00B2 ${colors.yellow(sprintf("%3.3f", round(Data.variance, 4)))}, ` +
            `σ ${colors.yellow(sprintf("%4.3f", round(Math.sqrt(Data.variance), 4)))}, ` + 
            `SE ±${colors.yellow(round(Data.standardError, 4))}, ` +
            `ME ±${colors.yellow(round(Data.marginError, 4))}, ` +
            `CI ${colors.yellow(round(Data.minCI, 4) + "%")} ~ ${colors.yellow(round(Data.maxCI, 4) + "%")}`
        );

        let totalFs = 0;
        let fsLevelStr = '';
        for (let i = 0; i < Data.fsCounts.length; i++) {
            fsLevelStr += sprintf ("%d = %s, ", i + 1, colors.yellow(round(Data.fsCounts[i] *100/Data.iterations, 4).toLocaleString() + '%'));
            totalFs += Data.fsCounts[i];
        }

        const hitMaxWin = Data.iterations/Data.maxWinCount;
        const hitRespin = Data.iterations/Data.respinCount;
        const hitFS     = Data.iterations/Data.fsTotalCount;
        const hitRsFreq = Data.iterations/Data.respinOccCount;

        console.log (`      Σ maxWin count = ${colors.yellow(Data.maxWinCount.toLocaleString())}, ` +  'hit = 1 in', colors.yellow(Math.round(hitMaxWin).toLocaleString()));
        console.log (`      Σ Re-Spin count = ${colors.yellow(Data.respinCount.toLocaleString())}, ` +  'Re-Spin Occurence = 1 in', colors.yellow(round(hitRsFreq, 3)));
        console.log ('      Σ FS:', colors.yellow(round(Data.sbCount * 100/Data.iterations, 2).toLocaleString() + '%'),
            'fs:', fsLevelStr.trim(), ` Σ free-spin-count = ${colors.yellow(Data.freespinCount.toLocaleString())}, ` +  'hit = 1 in', colors.yellow(round(hitFS, 3)));

        console.log ('      Σ', Object.keys(Data.reelSets).reduce ((acc, name) => {
            const p = round(Data.reelSets[name]*100/Data.iterations, 4).toLocaleString();
            return acc + `${shortenReelSetName(name)} = ${colors.yellow(p + "%")}, `;
        }, ''));

        // Show rounds per second
        const now = new Date();
        const currDuration = now - Data.startTS;
        console.log (`      rounds/sec =`, round(Data.iterations * 1000/currDuration, 2).toLocaleString());
    }
}

function shortenReelSetName(rsn) {
    let rsnStr = ``;
    if (!rsn)
        return rsnStr;

    const rsNameParts = rsn.split('_');

    rsnStr += rsNameParts[0];

    const lastPart = rsNameParts[rsNameParts.length - 1];
    const lastChars = lastPart.slice(-2);

    rsnStr += `_..` + lastChars;

    return rsnStr;
}

module.exports = Data;
