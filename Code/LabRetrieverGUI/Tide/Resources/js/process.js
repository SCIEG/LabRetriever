
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
    self._running.html("");
    self._myproc = Ti.Process.createProcess({
        args:val.split(' ')
    });
    self._myStatus = "running: " + val;
    $('#running').html(self._myStatus);
    var moreCmd = Ti.platform === "win32" ? ["C:\\Windows\\System32\\more.com"] : ["cat"];
    var more = Ti.Process.createProcess(moreCmd);

    more.setOnReadLine(function(data) {
        var s = $('#status');
        s.html(s.html() + "<br/>" + data.toString());
    });

    self._myproc.stdout.attach(more.stdin);
    self._myproc.launch();
    more.launch();
    self.running = true;
    self.interval = setInterval(self._checkStatus, 300);
}
SCIEG.Process._myStatusDot = " ."
SCIEG.Process._running = $('#running');
SCIEG.Process._checkStatus = function() {
    try {
    var self = SCIEG.Process;
    if (self._myproc.isRunning()) {
        self._running.html(self._myStatus + self._myStatusDot);
        if (self._myStatusDot.length == 5) {
            self._myStatusDot = " .";
        } else {
            self._myStatusDot += ".";
        }
    } else {
        self._running.html(self._myStatus + " ... Done.");
        self.running = false;
        clearInterval(self.interval);
    }
    } catch(e) {
        alert('exception: ' + e.toString());
        alert(document.body.innerHTML);
        clearInterval(self.interval);
    }
}
