SCIEG.exporter = {};

/**
 *
 * @return {Array} of file names created, it can be empty.
 */
SCIEG.exporter.createLabRcsv = function() {

    if (SCIEG.selectedSamples[3].length == 0 || SCIEG.selectedSamples[2].length == 0) {
        alert("You need to specify at least one Evidence sample and one Suspect sample");
        return [];
    }

    var files = [];

    var self = SCIEG.exporter;
    $.each($('input[name="pdo"]'), function(idx, val) {

        files.push(self._createFile());

        self.writeLine("alpha," + $('#alpha').val());
        self.writeLine("Drop-in rate," + $('#pdi').val());
        self.writeLine("Drop-out rate," + val.value);
        self.writeLine("IBD Probs," + $('#alleles0').val() + ',' + $('#alleles1').val() + ',' + $('#alleles2').val());
        self.writeLine("Race," + $('#race').val());

        for (var locus in SCIEG.selectedSamples[3][0]) {
            if (locus == 'name') continue;
            var evidence = SCIEG.selectedSamples[3][0][locus].slice(0);
            var unattributed = evidence.slice(0);
            var assumed = [];
            if (SCIEG.selectedSamples[1].length) {
                for (var i = 0; i < SCIEG.selectedSamples[1].length; i++) {
                    for (var k = 0; k < SCIEG.selectedSamples[1][i][locus].length; k++) {
                        var l = SCIEG.selectedSamples[1][i][locus][k];
                        if (unattributed.indexOf(l) != -1) {
                            unattributed.splice(unattributed.indexOf(l), 1);
                        }
                        if (evidence.indexOf(l) != -1 && assumed.indexOf(l) == -1) {
                            assumed.push(l);
                        }
                    }
                }
            }
            assumed.sort(SCIEG.exporter.sorter);
            self.writeLine(locus + "-Assumed," + assumed.join(','));

            for (var j = 0; j < SCIEG.selectedSamples[2].length; j++) {
                var suspects = [];
                for (var m = 0; m < SCIEG.selectedSamples[2][j][locus].length; m++) {
                    var n = SCIEG.selectedSamples[2][j][locus][m];
                    if (suspects.indexOf(n) == -1) {
                        suspects.push(n);
                    }
                }
                suspects.sort(SCIEG.exporter.sorter);
                self.writeLine(locus + "-Suspected," + suspects.join(','));
            }
            unattributed.sort(SCIEG.exporter.sorter);
            self.writeLine(locus + "-Unattributed," + unattributed.join(','));
        }
        self._out.close();
    });
    return files;
}

SCIEG.exporter.sorter = function(a,b){ return parseFloat(a) - parseFloat(b); };

SCIEG.exporter.writeLine = function(o) {
    SCIEG.exporter._out.writeLine(o);
}

SCIEG.exporter.filesCreated = 0;

SCIEG.exporter._createFile = function() {
    SCIEG.exporter._out = Ti.Filesystem.getFileStream(Ti.Filesystem.getApplicationDataDirectory(),
        "input" + SCIEG.exporter.filesCreated +".csv");
    //SCIEG.exporter.filesCreated++;
    SCIEG.exporter._out.open(Ti.Filesystem.MODE_WRITE);
    return Ti.Filesystem.getFile(Ti.Filesystem.getApplicationDataDirectory(),
        "input" + (SCIEG.exporter.filesCreated++) +".csv").nativePath();
}