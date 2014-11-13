SCIEG.exporter = {};

/**
 *
 * @return {Array} of file names created, it can be empty.
 */
SCIEG.exporter.createLabRcsv = function() {

    if (SCIEG.selectedSamples['detected'].length == 0 || SCIEG.selectedSamples['suspected'].length == 0) {
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
        self.writeLine("fst," + $('#fst').val());
        self.writeLine("IBD Probs," + $('#alleles0').val() + ',' + $('#alleles1').val() + ',' + $('#alleles2').val());
        self.writeLine("Race," + $('#race').val());

        for (var locus in SCIEG.selectedSamples['detected'][0]) {
            if (locus == 'loci' || locus == 'name') continue;
            var evidence = SCIEG.selectedSamples['detected'][0][locus].slice(0);
//            if (evidence.length == 0) continue;
            var unattributed = evidence.slice(0);
            var assumed = [];
            if (SCIEG.selectedSamples['assumed'].length) {
                for (var i = 0; i < SCIEG.selectedSamples['assumed'].length; i++) {
                    for (var k = 0; k < SCIEG.selectedSamples['assumed'][i][locus].length; k++) {
                        var l = SCIEG.selectedSamples['assumed'][i][locus][k];
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
            var locusName = (locus == 'VWA'?'vWA':locus)
            self.writeLine(locusName + "-Assumed," + assumed.join(','));

            var suspects = [];//move this into the loop if LabR can handle more than 1 suspect
            for (var j = 0; j < SCIEG.selectedSamples['suspected'].length; j++) {
                for (var m = 0; m < SCIEG.selectedSamples['suspected'][j][locus].length; m++) {
                    var n = SCIEG.selectedSamples['suspected'][j][locus][m];
                    suspects.push(n);
                }
            }
            //suspects.sort(SCIEG.exporter.sorter);
            self.writeLine(locusName + "-Suspected," + suspects.join(','));//move this line up if LabR can handle more than 1 suspect
//            console.log(suspects.join(','))

            unattributed.sort(SCIEG.exporter.sorter);
            self.writeLine(locusName + "-Unattributed," + unattributed.join(','));
        }
        self._out.close();
    });
    return files;
};

SCIEG.exporter.sorter = function(a,b){ return parseFloat(a) - parseFloat(b); };

SCIEG.exporter.writeLine = function(o) {
    SCIEG.exporter._out.writeLine(o);
};

SCIEG.exporter.filesCreated = 0;

SCIEG.exporter._createFile = function() {
    SCIEG.exporter._out = Ti.Filesystem.getFileStream(Ti.Filesystem.getApplicationDataDirectory(),
        "input" + SCIEG.exporter.filesCreated +".csv");
    //SCIEG.exporter.filesCreated++;
    SCIEG.exporter._out.open(Ti.Filesystem.MODE_WRITE);
    return Ti.Filesystem.getFile(Ti.Filesystem.getApplicationDataDirectory(),
        "input" + (SCIEG.exporter.filesCreated++) +".csv").nativePath();
};