(function() {
    var async  = require('async');
    var browserify = require('browserify');
    var exec   = require('child_process').exec;
    var fs     = require('fs');
    var glob   = require('glob');
    var less   = require('less');
    var mkdirp = require('mkdirp');
    var path   = require('path');

    function publish(filename, contents, done) {
        filepath = path.normalize(
            path.join(__dirname, '..', '..', 'modules', 'ui', 'build', filename)
        );

        mkdirp(path.dirname(filepath));
        fs.writeFile(filepath, contents, function(err) {
            if (!err) {
                console.log('Published', filename);
            }

            done(err);
        });
    }

    var tasks = [
        {
            description : 'html',
            files       : 'modules/ui/**/*.html',

            run: function(filenames, done) {
                publish(
                    'ui.html',
                    fs.readFileSync('modules/ui/html/ui.html'),
                    done
                );
            }
        },

        {
            description : 'javascript',
            files       : 'modules/ui/**/*.js',

            run: function(filenames, done) {
                browserify(
                    filenames.sort(function(a, b) {
                        var aBoot = a.indexOf('boot.js') > -1;
                        var bBoot = b.indexOf('boot.js') > -1;

                        if (aBoot && !bBoot) {
                            return -1;
                        } else if (!aBoot && bBoot) {
                            return 1;
                        } else {
                            return 0;
                        }
                    })
                ).bundle(function(err, output) {
                    err ? done(err) : publish('ui.js', output, done);
                });
            }
        },

        {
            description : 'styles',
            files       : 'modules/ui/**/*.less',

            run: function(filenames, done) {
                async.parallel(
                    filenames.map(function(filename) {
                        return function(rendered) {
                            less.render(
                                fs.readFileSync(filename).toString(),
                                {
                                    filename   : filename,
                                    compress   : true,
                                    strictMath : true
                                },
                                function (err, output) {
                                    rendered(err, output && output.css);
                                }
                            );
                        }
                    }),

                    function(err, outputs) {
                        (err && done(err)) || publish(
                            'ui.css',
                            outputs.join(''),
                            done
                        );
                    }
                );
            }
        }
    ];

    function build(options, done) {
        async.parallel(
            tasks.map(function(task) {
                return function(done) {
                    glob(task.files, { }, function(err, filenames) {
                        (err && done(err)) || task.run(filenames, done);
                    });
                }
            }),

            done
        );
    }

    module.exports = {
        build: build
    };
})();
