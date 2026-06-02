const colors        = require ('colors');
const Config        = require ('./config')();
const { program }   = require('commander');
const log           = require ('./common/log');
const Nats          = require ('./common/nats-core');
const RNGController = require ('./rng-ctrl');
const GRPCServer    = require ('./grpc/server');

program
      .showHelpAfterError()
      .option('-n, --with-nats', 'run with NATS server', false)
      .parse (process.argv)
;

const options = program.opts ();
let rng, grpcServer;

process.on ('SIGINT', async () => {
    console.log (colors.red ('Interrupted.'), 'waiting for grpc server to stop...');
    grpcServer.stop();
    setTimeout(() => process.exit(0), 500);
});

async function main () {
    rng = new RNGController();

    await rng.init ();

    if (options.withNats) {
        await Nats.init ();
        Nats.openChannelBinary (Config.ClientChannelBinary, handleClientBinary);
        Nats.openChannel (Config.ClientChannelJson, handleClientJson);
        log.info ('NATS initialized');
    }

    grpcServer = new GRPCServer();
    grpcServer.start();
}

function handleClientBinary (err, sub, payload, reply, msg) {
    const data = new Uint8Array(payload);

    try {
        if (err)
            throw err;

        const response = rng.getScaledBinary (data);
        const buffer = new Uint8Array(response);
        Nats.sendRaw (reply, buffer);
    }
    catch (err) {
        log.error ({ err, sub, data }, 'handleClientBinary: error handling request');
        const view   = new DataView(data);

        // set error in header to 1
        view.setBigUint64(10, 1, false);
        Nats.send(reply, data);
    }
}

function handleClientJson (err, sub, payload, reply, msg) {
    let response;

    const { header, body } = payload;
    const { seq, req } = header || {};

    try {
        if (err)
            throw err;

        switch (req) {
            case 'scaled':
                response = rng.getScaled (body);
                break;

            case 'scaledV':
                response = rng.getScaledV (body);
                break;

            default:
                throw new Error (`unrecognized request "${rea}"`);
        }

        const replyPayload = {
            header : { seq, req },
            body : response
        };

        Nats.send (reply, replyPayload);
    }
    catch (err) {
        log.error ({ err, sub, payload }, 'handleClientJson: error handling request');
        Nats.send(reply, 
            {
                header : { seq, req, error: err.message }
            }
        );
    }
}

main ()
    .then (() => {
        log.info ('RNG service init OK');
    })
    .catch ((err) => {
        console.log (colors.red ('FATAL ERROR:'), err);
        process.exit (1);
    })
;
