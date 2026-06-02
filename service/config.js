const env = process.env;

if (typeof env.GAME_ID === 'undefined')
    throw new Error ('env "GAME_ID" undefined');

if (typeof env.NUM_THREADS === 'undefined')
    throw new Error ('env "NUM_THREADS" undefined');

if (typeof env.REQ_MAX_AGE === 'undefined')
    throw new Error ('env "REQ_MAX_AGE" undefined');

if (typeof env.ENABLE_EXPRESS === 'undefined')
    throw new Error ('env "ENABLE_EXPRESS" undefined');

if (env.ENABLE_EXPRESS === 'true' && typeof env.EXPRESS_PORT === 'undefined')
    throw new Error ('env "EXPRESS_PORT" undefined');

if (typeof env.RNG_MAX_RECORDS === 'undefined')
    throw new Error ('env "RNG_MAX_RECORDS" undefined');

if (typeof env.RNG_MONITOR_CHANNEL === 'undefined')
    throw new Error ('env "RNG_MONITOR_CHANNEL" undefined');

if (typeof env.RNG_MAX_HOLD_TIME === 'undefined')
    throw new Error ('env "RNG_MAX_HOLD_TIME" undefined');

function toNumber (x) {
    const num = Number(x);

    if (Number.isNaN(num))
        throw new Error (`invalid value "${x}" : number expected`);

    return num;
}

module.exports = () => {
    return {
        name       : 'game-engine',
        id         : env.GAME_ID,
        gameCh     : `ge.${env.GAME_ID}`,
        numThreads : toNumber(env.NUM_THREADS),
        http: {
            enable : env.ENABLE_EXPRESS === 'yes',
            port   : toNumber(env.EXPRESS_PORT),
        },
        game: {
            reqMaxAge : toNumber(env.REQ_MAX_AGE),
        },
        nats: {
            host : env.NATS_HOST,
            port : env.NATS_PORT || 4222,
        },
        log: {
            level : 'debug'
        },
        rng: {
            maxRecords: toNumber(env.RNG_MAX_RECORDS),
            maxHoldTime: toNumber(env.RNG_MAX_HOLD_TIME),
            monitorChannel: env.RNG_MONITOR_CHANNEL,
        },
    };
};
