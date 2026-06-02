const colors  = require('colors');
const express = require('express');
const app     = express();

class Server {
    static app = app;
    static init ({ port }) {
        const host = '0.0.0.0';

        return new Promise ((resolve, reject) => {
            const httpServer = app.listen (port, host, (err) => {
                if (err)
                    return reject(err);

                console.log(`Server is running on http://${host}:${port}`);
                resolve ();
            });

            httpServer.on ('close', () => {
                console.log (`${colors.yelow('server is shutdown')}`)
            });
        });
    }
}

module.exports = Server;
