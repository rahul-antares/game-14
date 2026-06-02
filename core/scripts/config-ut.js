#!/bin/env node

const os                  = require('os');
const fs                  = require('fs');
const path                = require('path');
const colors              = require('colors');
const sprintf             = require('sprintf-js').sprintf;
const { program }         = require('commander');
const beautify            = require("json-beautify");

program
    .showHelpAfterError()
    .option('-i, --include <module...>', 'relative path of UT modules to include (can specify multiple options)')
    .option('-x, --exclude <module...>', 'relative path of UT modules to exclude (can specify multiple options)')
    .option('-v, --variant <variant>', '96 or 94', '96')
    .option('-V, --verbose', 'verbose')
    .option('-l, --list-modules', 'list modules')
    .option('-q, --quiet', 'show only errors')
    .parse (process.argv)
;

const utModulesDir = path.join(__dirname, 'config-modules');
const options = program.opts ();
const { GameWrapper } = require(`../index${options.variant}`);

if (options.listModules) {

    //
    // Just list the modules and exit
    //

    let fpath;
    const files = fs.readdirSync (utModulesDir);

    try {
        for (let file of files) {
            if (!file.match(/\.js$/))
                continue;
            fpath = path.join (utModulesDir, file);
            const t = require (fpath);
            const f = file.replace (/\.js$/, '');
            console.log (sprintf('module %\'.32s ( %40s ) @ %s',
                colors.green(f), colors.yellow(t.name), fpath
            ));
        }
    }
    catch (err) {
        console.error (colors.red ('Error:'), 'modules =', fpath);
        console.error (err);
    }

    process.exit (0);
}

const tests = [];

function matchPath (pathArr, file) {
    return pathArr.find ((p) => {
        const regExp = new RegExp (`${file}$`)
        if (p.match (regExp))
            return true;
        return false;
    });
}

function loadTests () {
    console.log ('searching directory', colors.yellow(utModulesDir), 'for modules ...');

    const files = fs.readdirSync (utModulesDir);
    for (let file of files) {
        try {
            if (!file.match (/\.js$/))
                continue;

            if (options.include?.length) {
                if (!matchPath (options.include, file))
                    continue
            }

            if (options.exclude?.length) {
                if (matchPath (options.exclude, file))
                    continue
            }

            const fpath = path.join (utModulesDir, file);
            const t = require (fpath);
            tests.push ({ ...t, file: fpath });
            console.log (`      loaded unit ${colors.yellow (file)} [ "${colors.magenta(t.name)}" ] ...`);
        }
        catch (err) {
            console.error (colors.red ('Error loading module'), colors.yellow(file));
            throw err;
        }
    }
}

function runTests (config) {
    for (let i = 0; i < tests.length; i++) {
        const { name, file, func } = tests[i];

        if (options.verbose) console.log (`running test ${colors.yellow(name)} (file ${file}) ...`);

        const resultStr = func (config);

        if (resultStr) {
            console.error (colors.yellow (name), colors.bgRed ('Failed:'), colors.reset (resultStr));
            console.error (`(test module => ${colors.yellow (file)})`)
            throw new Error ('test failed')
        }
        else if (!options.quiet) {
            console.log ('test', colors.yellow (name), colors.green('passed'));
        }
    }
}

let count = 0;
async function main () {
    loadTests ();

    const config = GameWrapper.getConfig();
    console.log(config);
    runTests (config);
}

main ()
    .then (() => {
        process.exit (0);
    })
    .catch ((err) => {
        console.error (`(Iteration #${count.toLocaleString()})`, colors.red ('Error:'), err);
        process.exit (1);
    })
;