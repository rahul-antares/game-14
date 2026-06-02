const express    = require ('express');
const Config     = require ('../config');
const log        = require ('../common/log').child ({ module: 'express' });
const ThreadCtrl = require ('./thread-ctrl');
const Message    = require ('./message');

function init () {
    const app  = express();
    const port = Config().http.port;

    app.get('/play', async (req, res) => {
        const betAmount    = parseInt (req.query.betAmount);
        const refBetAmount = parseInt (req.query.refBetAmount);
        const featureMode  = parseInt (req.query.featureMode);
        const featureBuy   = (req.query.featureBuy === 'true');

        const m = Message.makeRequest ('play', {
            betAmount, refBetAmount, featureMode, featureBuy,
        });

        const response = await ThreadCtrl.submit (m);

        res.json (response);
    });

    app.listen(port, () => {
        log.info (`Express server running at http://localhost:${port}`);
    });
}

module.exports = { init };
