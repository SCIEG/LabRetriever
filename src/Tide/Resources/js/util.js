
if (typeof window.SCIEG == "undefined") window.SCIEG = {};

SCIEG.util = {};

SCIEG.util.busy = function() {
    $('#busy').removeClass("hidden");
    return function(){
        $('#busy').addClass("hidden");
    }
}

SCIEG.util.status = function(msg) {
    $('#running').html(msg).css('opacity', 1);
    setTimeout(function(){$('#running').animate({'opacity':0.4}, 2000);}, 2000);
}