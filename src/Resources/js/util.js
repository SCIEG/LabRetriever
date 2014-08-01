
if (typeof window.SCIEG == "undefined") window.SCIEG = {};

SCIEG.util = {};

SCIEG.util.busy = function() {
    $('#busy').removeClass("hidden");
    return function(){
        $('#busy').addClass("hidden");
    }
};

SCIEG.util.fileNames = [];

SCIEG.util.status = function(msg, filePath) {
    var $status = $('#running');
    if(filePath) {
        var fileName = filePath.replace(/^.*(\\|\/|\:)/, '');
        SCIEG.util.fileNames.push(fileName);
        var content = "<ol>";
        $.each(SCIEG.util.fileNames, function(index, name) {
            content += "<li>" + name + "</li>";
        });
        content += "</ol>";
    }
    $status.html(msg).css('opacity', 1);
    $status.attr("title", "File(s) Loaded");
    $status.attr("data-content", content);
    $status.popover("destroy");
    $status.popover({placement: "right", html: true, trigger: "hover"});
    setTimeout(function(){$('#running').animate({'opacity':0.4}, 2000);}, 2000);
};