#!/bin/env node

const fs                  = require('fs');
const colors              = require('colors');
const { program }         = require('commander');
const beautify            = require("json-beautify");
const RNG_Wrap            = require('../index');

program
	.showHelpAfterError()
	.option('-v, --version', 'show version')
    .option('-s, --scaled <max>', 'get scaled <max>', parseInt)
	.parse (process.argv)
;

const options = program.opts ();

if (!options.version && !options.scaled)
    program.help();

const RNG = new RNG_Wrap();

if (options.version) {
    const version = RNG.getVersion();
	console.log (beautify (version, null, 2, 1000));
}

if (options.scaled) {
    const scaled = RNG.getScaled(options.scaled);
	console.log (scaled.s);
}

process.exit(0);
