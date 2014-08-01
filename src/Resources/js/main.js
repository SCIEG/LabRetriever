$().ready( function () {
    if(window.location.href.indexOf("noSplash") == -1) {
        setTimeout( function () {
            $( "#splash" ).fadeOut( 750, function () {
                $( '#splash' ).hide()
            } )
        }, 3500 );
    } else {
        $( '#splash' ).remove();
    }

    SCIEG.totalResultsTriesToAtempt = 4400;
    SCIEG.resultsInternalTime = 250;

    $( '#runner' ).click( function () {
        SCIEG.toSave = [
            ["", "Detected", "Unattributed", "Assumed", "Suspected", "", "Likelihood Ratio"]
        ];
        var busy = SCIEG.util.busy();
        try {
            var files = SCIEG.exporter.createLabRcsv();
        } catch ( e ) {
            log( "creating csv failed: " + e.toString() );
            files = [];
        }
        if ( files.length == 0 ) busy();
        SCIEG.currentResults = [];
        var executable = Ti.Filesystem.getFile( Ti.Filesystem.getApplicationDirectory(), "labr/labr" ).nativePath();
        var likelihood1OptionText =  $( "#likelihood1 option:selected").text();
        var likelihood2OptionText = $( "#likelihood2 option:selected").text();
        try {
            for ( var i = 0; i < files.length; i++ ) {
                var outputName = "output" + (new Date().getTime()) + ".csv";
                var process = [executable, files[i],
                    Ti.Filesystem.getFile( Ti.Filesystem.getApplicationDataDirectory(), outputName ).nativePath(),
                    $( "#likelihood1" ).val(), $( "#likelihood2" ).val()];
                //log("run: " + process);
                // delete output file before running, as to not pick up previous run's results
                var outFile = Ti.Filesystem.getFile( Ti.Filesystem.getApplicationDataDirectory(), outputName );
                if ( outFile.exists() ) {
                    var d = outFile.deleteFile();
                    if ( !d ) {
                        alert( "Failed to delete the output file prior to run.\n" +
                            d + "\n" +
                            "Please delete this file before attempting another run:\n " +
                            outFile.nativePath() );
                        return;
                    }
                }
                console.log(process)
                SCIEG.Process.runProcess( process );
                SCIEG.resultsFoundTries = 0;
                SCIEG.toSave = [
                    ['Case ID: ' + $( '#caseInput' ).val()],
                    ['Sample ID: ' + $( '#sampleInput' ).val()],
                    ['Analyst: ' + $( '#analystInput' ).val()],
                    []
                ];

                SCIEG.saveRowOffset = 4;
                $( '#inputs tr' ).each( function ( i, v ) {
                    if ( i == 1 ) return;
                    var row = [];
                    $( v ).find( 'td' ).each( function ( ii, vv ) {
                        var t = $( vv ).text();
                        if ( i == 2 ) {
                            t = t.replace( /remove$/, '' );
                        } else {
                            t = t.replace( /[^\dA-Za-z]/g, ' ' )
                        }
                        row.push( '"' + t + '"' )
                    } );
                    SCIEG.toSave.push( row );

                } );
                var lastRow = [];
                $.each( SCIEG.toSave[SCIEG.saveRowOffset], function () {
                    lastRow.push( "" );
                } );
                SCIEG.toSave.push( lastRow );
                SCIEG.toSave.push( [""] );
                SCIEG.toSave.push( ["P(DI)", "P(DO)", "IBD Probabilities"] );
                SCIEG.toSave.push( [ $( '#pdi' ).val(), $( 'input[name="pdo"]' ).val(),
                        $( '#alleles0' ).val() + ", " + $( '#alleles1' ).val() + ", " + $( '#alleles2' ).val()] );

                var file_dir = Ti.Filesystem.getApplicationDataDirectory();
                runResultsTest(outputName, process, busy, file_dir, likelihood1OptionText, likelihood2OptionText);

            }
        } catch ( e ) {
            log( e.toString() );
            busy();
        }
    } );

    var runResultsTest = function(outputName, process, busy, file_dir, likelihood1, likelihood2) {
        var _this = this;
        SCIEG.resultsInterval = setTimeout( function () {
            var f = Ti.Filesystem.getFile(file_dir , outputName );
            if ( f.exists() ) {
                clearTimeout( SCIEG.resultsInterval );
                var results = f.read();
                SCIEG.currentResults.push( results );
                var data = _this.outputParse( results.toString() );
                $( '#saveBtn' ).removeClass( "hidden" );
                displayOutput( data );
                $( '#result tr' ).each( function ( i, v ) {
                    if ( i == 0 ) {
                        SCIEG.toSave[SCIEG.saveRowOffset].push( "" );
                        SCIEG.toSave[SCIEG.saveRowOffset].push( "Likelihood Ratio" );
                        return;
                    }
                    if ( i == 2 ) return;
                    if ( i > 2 ) i--;
                    SCIEG.toSave[i + SCIEG.saveRowOffset].push( "" );
                    $( v ).find( 'td' ).each( function ( ii, vv ) {
                        SCIEG.toSave[i + SCIEG.saveRowOffset].push( vv.textContent );
                    } );
                } );
                addWhereTheStatsComeFromFooter(likelihood1, likelihood2);
                busy();
            } else {
                clearTimeout( SCIEG.resultsInterval );
                if ( SCIEG.resultsFoundTries++ > SCIEG.totalResultsTriesToAtempt ) {
                    log( "giving up looking for results" );
                    SCIEG.util.status( "Calculation took too long, it may have died? Try again." );
                    busy();
                    SCIEG.Process.kill();
                } else {
                    updateResultsInterval();
                    runResultsTest(outputName, process, busy, file_dir);
                }
            }
        }, SCIEG.resultsInternalTime );
    };

    var updateResultsInterval = function() {
        if(SCIEG.resultsFoundTries > 11) {
            //if 3 seconds go by up the wait to 1 second
            SCIEG.resultsInternalTime = 6000;
        } if(SCIEG.resultsFoundTries > 130) {
            //if 33 seconds go by up the wait to 1 minute
            SCIEG.resultsInternalTime = 60000;
        } else if(SCIEG.resultsFoundTries > 190) {
            //in an hour goes by then check every 5 minutes
            SCIEG.resultsInternalTime = 300000;
        }
    };

    $( '#loadFile' ).click( function () {
        try {
            Ti.UI.currentWindow.openFileChooserDialog( function ( names ) {
                var busy = SCIEG.util.busy();
                try {
                    if ( names.length == 0 ) {
                        // user hit cancel
                        busy();
                        return;
                    }
                    var name = names[0];
                    if ( name.length == 0 ) {
                        // somehow didn't enter a file name?
                        busy();
                        return;
                    }
                    var f = Ti.Filesystem.getFile( name );
                    if ( f.exists() ) {
                        if ( SCIEG.fileData === undefined ){
                            SCIEG.fileData = [];
                        }

                        var data = window.load( f.nativePath() );

                        if ( data ) {
                            SCIEG.fileData = SCIEG.fileData.concat( data.splice( 0 ) );
                            fileLoaded(name);
                            calculateUnattributed();
                            $( '.loadFile' ).html( 'load another file' );
                            busy();
                            return;
                        }

                    }
                    busy();
                } catch ( e ) {
                    log( "Caught Error: " + e.toString() );
                    busy();
                }
            }, {multiple: false} );
        } catch ( e ) {
            log( 'file chooser error' + e.toString() );
        }
    } );

    $( "#ibd_probabilities" ).click( function ( evt ) {
        var ul = $( evt.target ).next();
        if ( ul.height() ) {
            ul.animate( {height: 0}, 1000 );
        } else {
            ul.animate( {height: ul[0].scrollHeight}, 1000 );
        }
    } );

    var addWhereTheStatsComeFromFooter = function(likelihood1, likelihood2) {
        SCIEG.toSave.push( [""] );
        SCIEG.toSave.push( ["Hypothesis 1: " + likelihood1] );
        SCIEG.toSave.push( ["Hypothesis 2: " + likelihood2] );
        SCIEG.toSave.push( [""] );
        SCIEG.toSave.push( ["\"NIST 1036 U.S. Population Dataset - 29 autosomal STR loci and 23 Y-STR loci found at http://www.cstl.nist.gov/div831/strbase/NISTpop.htm\""] );
        SCIEG.toSave.push( ["\"Butler\, J.M.\, Hill, C.R.\, Coble, M.D. (2012) Variability of new STR loci and kits in U.S. population groups.  Profiles in DNA http://www.cstl.nist.gov/div831/strbase/pub_pres/Profiles-in-DNA_Variability-of-New-STR-Loci.pdf\""] );
        SCIEG.toSave.push( ["http://www.promega.com/resources/articles/profiles-in-dna/2012/variability-of-new-str-loci-and-kits-in-us-population-groups/"] );
    };

    $( '#saveBtn span.button' ).click( function () {

        Ti.UI.currentWindow.openSaveAsDialog( function ( paths ) {
                writeOutput( paths[0], SCIEG.toSave );
            },
            {      title:    'Lab Retriever results',
                multiple:    false,
                types:       ['csv'],
                defaultName: getISODate() + " " +
                    $( $( '#inputs td.sampleName' )[0] ).text().replace( /[^A-Za-z\d ]/g, '' ).replace( /remove$/, '' ) + " " +
                    $( "#likelihood1" ).val().split( "" ).join( "S-" ) + "UNK " +
                    $( "#likelihood2" ).val().split( "" ).join( "S-" ) + "UNK.csv"
            } )
    } );
} );