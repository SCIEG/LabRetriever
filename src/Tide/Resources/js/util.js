
if (typeof window.SCIEG == "undefined") window.SCIEG = {};

SCIEG.util = {};

SCIEG.util.busy = function() {
    $('#busy').removeClass("hidden");
    return function(){
        $('#busy').addClass("hidden");
    }
}

SCIEG.util.tempStatus = function(msg) {
    $('#running').html(msg);
    setTimeout(function(){$('#running').fadeOut(2000, function(){
        var r = $('#running');
        if (r.html() == msg) r.html('');
    })}, 2000);
}

SCIEG.util.status = function(msg) {
    $('#running').html(msg);
}