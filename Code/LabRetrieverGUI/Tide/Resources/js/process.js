
if (typeof window.SCIEG == "undefined") window.SCIEG = {};
SCIEG.Process = {
    running: false
};

SCIEG.Process.runProcess = function(val) {
    var self = SCIEG.Process;
    if (self.running) {
        alert('process already running: ' + self._myStatus);
        return;
    }
    self._myproc = Ti.Process.createProcess({
        args:val
    });
    self._myStatus = "running: " + val.join(' ');
    log(self._myStatus);
    var moreCmd = Ti.platform === "win32" ? ["C:\\Windows\\System32\\more.com"] : ["cat"];
    var more = Ti.Process.createProcess(moreCmd);

    more.setOnReadLine(function(data) {
        log(s.html() + "<br/>" + data.toString());
    });

    self._myproc.stdout.attach(more.stdin);
    self._myproc.launch();
    more.launch();
    self.running = true;
    self.interval = setInterval(self._checkStatus, 300);
}
SCIEG.Process._checkStatus = function() {
    try {
    var self = SCIEG.Process;
    if (!self._myproc.isRunning()) {
        log(" ... Done.");
        self.running = false;
        clearInterval(self.interval);
    }
    } catch(e) {
        log('exception checking status: ' + e.toString());
        clearInterval(self.interval);
    }
}
