const { threadId,
    parentPort,
workerData }          = require ('node:worker_threads');
const log             = require ('../common/log').child ({ thread: threadId });
const Message         = require ('./message');

let seq = 0;
let GameWrapper;

function main () {
    return new Promise ((resolve, reject) => {
        GameWrapper = require (`@core/game/${workerData.gameEntryIndexFile}`).GameWrapper;
        const readyMsg = Message.makeInfo ('ready', {});
        parentPort.postMessage (readyMsg.serialize ());

        parentPort.on ('message', handleMsg);
    });
}

function handleMsg (msg) {
    const { seq, id, data } = msg;
    let response;

    try {
        switch (id) {
            case 'init':
                response = GameWrapper.init(data.options);
                break;

            case 'play':
                response = GameWrapper.play(data);
                break;

            case 'config':
                response = GameWrapper.getConfig();
                break;

            case 'reseed':
                GameWrapper.reseed();
                response = { status: 0 };
                break;

            case 'next':
                response = GameWrapper.next(data.options, data.prevState);
                break;

            default:
                log.error ({ m:msg }, `RX unknown message id "${id}"`);
                throw new Error ('unknown message id');
        }

        const message = Message.makeResponse (id, seq, response);
        parentPort.postMessage (message.serialize ());
    }
    catch (err) {
        log.error ({ err, msg }, 'handle message failed');

        const message = Message.makeResponse (id, seq, null, err.message || 'unknown error');
        parentPort.postMessage (message.serialize ());
    }
}

let shouldStop = false;
const sleep = ms => new Promise(r => setTimeout(r, ms));
function stop (options) {
    shouldStop = true;
}

main ()
    .then (() => {
        process.exit (0);
    })
