const { spawn } = require('child_process');
const path      = require('node:path');
const log       = require('../common/log').child({ module: 'grpc/server' });

const SERVERPATH = './build/server';

class ServerManager {
    constructor() {
        this.process = null;
    }

    start() {
        if (this.process) {
            log.warn ('already running');
            return;
        }

        this.process = spawn(path.join (__dirname, SERVERPATH), [], { stdio: 'inherit' });

        this.process.on('spawn', (err) => {
            log.info ('GRPC server start OK');
        });

        this.process.on('error', (err) => {
            log.error({ err }, 'server error');
            this.process = null;
        });

        this.process.on('exit', (code, signal) => {
            log.info ({ code, signal }, 'server exited');
            this.process = null;
        });
    }

    stop() {
        log.warn ('Stopping server...');

        if (this.process) {
            this.process.kill('SIGTERM');
            this.process = null;
        }
    }
}

module.exports = ServerManager;
