const bunyan = require('bunyan');
const Config = require('../config')();

const name  = Config.name;
const level = Config.log.level || 'debug';

const log = bunyan.createLogger({
    name: `${name}.srv`,
    streams: [],
    serializers: { err: bunyan.stdSerializers.err }
});

log.addStream ({
    name: 'stdout',
    stream: process.stdout,
    level,
});

const child_method = log.child;
const children = [];

log.child = function(options) {
    let module = options.module;

    let __child = child_method.call(this, options);

    if(!module) {
        children.push({
            module: module,
            child: __child
        });
    }

    return __child;
}

log.set_level = function(stream, level, module) {
    for(let i=0; i<children.length; i++){
        if(module && (module !== children[i].module))
                continue;

        children[i].child.levels(stream, level);
    }
};

module.exports = log;
