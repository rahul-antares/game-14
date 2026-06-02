const { threadId,
parentPort,
workerData }      = require('node:worker_threads');
const colors      = require('colors');
const { Harness } = require(`../../../build/lib/harness${workerData.variant}.node`);

let seq = 0;

function logToStdError (...args) {
    console.error (colors.red(`[ thread ${threadId} ]`), ...args);
}

function logError (...args) {
    logToStdError(colors.red ('Error:'), ...args);
}

function main () {
    return new Promise ((resolve, reject) => {
        parentPort.postMessage (createMsg('ready'));

        parentPort.on ('message', (msg) => {
            const { header, body } = msg;
            const { type } = header;

            switch (type) {
                case 'start-batch':
                    run (body).finally (() => resolve());
                    break;

                case 'stop':
                    stop (body);
                    break;

            default:
                logError (`unknown message type "${type}" RX from child thread`);
                logError ('the full message is:', msg);
            }
        });
    });
}

let shouldStop = false;
const sleep = ms => new Promise(r => setTimeout(r, ms));
function stop (options) {
    shouldStop = true;
}

async function run (options) {
    const { batchSize, playOpts } = options;
    const input = {
        iterations: batchSize,
        playOptions: playOpts,
        nextOpts: {
            gamble: options.next === 'gamble',
        },
    };

    try {
        const harness = new Harness();

        console.error ('using RNG API', colors.yellow(harness.getRNGLibType().type));
        harness.initRNG({
            nats: {
                url : 'nats://localhost:4222',
                rngCh: 'rng.binary.client',
                timeout : 2000,
                name: 'game2',
                verbose : false,
                allowReconnect : true,
                retryOnFailedConnect : true,
                maxReconnect : 50,
                reconnectWait : 500,
                pingInterval : 5000,
                maxPingsOut : 5,
            },
            grpc: {
                host: 'localhost',
                port: 50051
            }
        });

        do {
            harness.run ({ ...input, iterations: batchSize, clearMaxWin: true  })
            parentPort.postMessage (createMsg('data', harness.getStats()));
            await sleep (0) // to allow signal handler to work
        } while (!shouldStop);

        logToStdError('exiting ...')
    }
    catch (err) {
        logError('Error:', err);
    }
}

function createMsg (type, msg) {
    return {
        header: {
            seq: seq++,
            type: type,
        },
        body: msg,
    };
}

main ()
    .then (() => {
        process.exit (0);
    })
