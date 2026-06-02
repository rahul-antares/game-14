const { v4: uuidv4 }  = require ('uuid');
const colors          = require ('colors');
const Config          = require ('./config');
const { program }     = require ('commander');
const log             = require ('./common/log');
const Nats            = require ('./common/nats-core');
const Express         = require ('./lib/express');
const ThreadCtrl      = require ('./lib/thread-ctrl');
const Message         = require ('./lib/message');

program
    .showHelpAfterError()
    .option('-i --game-entry-index-file <name>', 'name of the game modules index file (defaults to "index")', "index")
    .parse (process.argv)
;

const options = program.opts ();

/*
 * TODO: do this better for docker env
process.on ('SIGINT', async () => {
    console.log (colors.red ('Interrupted...'));
    process.exit(0);
});
*/

async function main () {
    const instanceId = uuidv4 ();

    ThreadCtrl.start ({
        numThreads: Config().numThreads,
        instanceId,
        gameEntryIndexFile: options.gameEntryIndexFile,
    })

    await Nats.init ({
        servers : `${Config().nats.host}:${Config().nats.port}`
    });

    Nats.openChannel (Config().gameCh, handleGameReq, { subscribe: { queue: Config().id }});

    log.info (`Nats: subscribed to channel "${Config().gameCh}"`);
    log.info (`Game engine "${Config().id}" (instance id ${instanceId}) init OK`);

    if (Config().http.enable)
        Express.init ();
}

function handleGameReq (_err, sub, _data, reply, msg) {
    (async () => {
        try {

            const m = Message.makeRequest (_data.data.method, _data.data, { reply });
            if (_err)
                throw _err;

            const response = await ThreadCtrl.submit (m);

            if (m.id !== 'reseed')
                Nats.send (reply, response);
        }
        catch (err) {
            const errStr = (err instanceof Error) ? err.message:
                (typeof err === 'string') ? err : 'game engine error';

            log.error ({ err, sub, _data }, 'handleGameReq: error handling request');

            if (reply)
                Nats.send (reply, { error: {
                    message   : errStr,
                    errorCode : 'ERR_GAME_ENGINE',
                    status    : 400
                }});
        }
    })();
}

main ()
    .then (() => {
    })
    .catch ((err) => {
        console.error (colors.red ('FATAL ERROR: app init failed:'), err);
        process.exit (1);
    })
;
