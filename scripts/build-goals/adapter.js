const async  = require('async');
const spawn  = require('child_process').spawn;
const fs     = require('fs');
const glob   = require('glob');
const less   = require('less');
const mkdirp = require('mkdirp');
const path   = require('path');

const rootDir          = path.normalize(path.join(__dirname, '..', '..'));
const adapterDir       = path.join(rootDir, 'modules', 'adapter');
const engineDir        = path.join(rootDir, 'modules', 'engine');
const engineIncludeDir = path.join(engineDir, 'include', 'randar');
const engineSrcDir     = path.join(engineDir, 'src');

function publish(filename, contents, sensitive, done) {
    const filepath = path.normalize(path.join(adapterDir, filename));

    function write() {
        mkdirp(path.dirname(filepath));
        fs.writeFile(filepath, contents, (err) => {
            if (!err) {
                console.log('Published', filepath);
            }
            done(err);
        });
    }

    /**
     * If this file is sensitive to updates, only publish it when its
     * contents have changed.
     *
     * For example, node-gyp will perform an unconditional full compilation
     * if the binding file has changed, regardless if a partial compilation
     * is logically possible.
     */
    if (sensitive) {
        fs.readFile(filepath, (err, data) => {
            if (data && data.toString() == contents) {
                done();
            } else {
                write();
            }
        });
    }

    else {
        write();
    }
}

function run(command, args, expectedFilename, done) {
    const program = spawn(command, args, {
        cwd   : adapterDir,
        stdio : 'inherit'
    });

    program.on('error', done);
    program.on('close', (err) => {
        if (!err) {
            console.log('Published', path.join(adapterDir, expectedFilename));
        }
        done(err);
    });
}

function build(options, done) {
    const swigFilename   = 'engine.i';
    const wrapFilename   = 'engine_wrap.cxx';
    const gypFilename    = 'binding.gyp';
    const moduleFilename = 'build/engine.node';

    const headers = glob.sync(path.join(engineIncludeDir, '**', '*.hpp'));
    const sources = glob.sync(path.join(engineSrcDir, '**', '*.cpp'));

    headerContents = headers.reduce((result, filename) => {
        result[filename] = fs.readFileSync(filename).toString();
        return result;
    }, { });

    // Sort headers by dependencies required. If we provide these headers to
    // swig unsorted it will still create the c++ wrapper, but it can't confirm
    // all dependencies have been met -- so we'll see a bunch of scary warnings.
    var availableHeaders = headers.slice();
    var circularHeaders  = [];
    var sortedHeaders    = [];
    while (availableHeaders.length) {
        var key = availableHeaders.findIndex((header) => {
            return availableHeaders.every((otherHeader) => {
                return headerContents[header].indexOf(
                    otherHeader.replace(engineIncludeDir, '')
                ) == -1;
            });
        });

        // If we can't find a header without satisfied dependencies, we've
        // encountered a circular dependency. Swig 
        if (key == -1) {
            key = 0;
            circularHeaders.push(availableHeaders[key]);
        }
        sortedHeaders = sortedHeaders.concat(
            availableHeaders.splice(key, 1)
        );
    }

    if (circularHeaders.length) {
        console.warn('! Detected circular dependencies in');
        circularHeaders.forEach((header) => console.warn('! -', header));
        console.warn('! Build will continue but may fail');
    }

    // Start the swig file with the main module declaration.
    var swigContents = [
        '%module engine',
        '%{',
    ].concat(sortedHeaders.map((filename) => {
        return '#include "' + filename + '"';
    })).concat([
        '%}'
    ]).concat(sortedHeaders.map((filename) => {
        return '%include "' + filename + '"';
    })).join('\n');

    // Describe the complete compilation of the engine node module.
    const gypBinding = {
        targets: [{
            target_name: 'engine',

            sources: sources.map((filename) => {
                return filename.replace(
                    engineDir,
                    path.join('..', 'engine')
                )
            }).concat([wrapFilename]),

            include_dirs: [
                'modules/engine/include',
                'modules/engine/platform/linux/include',
                'modules/engine/platform/linux/include/cef',
                'modules/engine/include/bullet3'
            ].map((dir) => path.normalize(path.join(rootDir, dir))),

            libraries: [
                '-Llib',
                '-lX11',
                '-lXxf86vm',
                '-pthread',
                '-lXi',
                '-lXrandr',
                '-lGL',
                '-lGLEW',
                '-lBulletDynamics',
                '-lBulletCollision',
                '-lLinearMath',
                '-lpng'
            ],

            // Disable warnings.
            cflags: ['-w'],

            // Enable exceptions.
            'cflags!'    : ['-fno-exceptions'],
            'cflags_cc!' : ['-fno-exceptions']
        }]
    };

    async.series([
        (next) => publish(swigFilename, swigContents, false, next),
        (next) => publish(
            gypFilename, JSON.stringify(gypBinding, null, '    '), true, next
        ),

        // Creates a C++ file that wraps our engine within V8-friendly code.
        (next) => run(
            'swig',
            ['-c++', '-javascript', '-node', '-o', wrapFilename, swigFilename],
            swigFilename,
            next
        ),

        // Creates an importable node module for our engine.
        (next) => run(
            'node-gyp',
            ['build', '-j', '4'],
            moduleFilename,
            next
        )
    ], done);
}

module.exports = {
    build: build
};
