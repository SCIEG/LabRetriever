
function getISODate() {
    var d = new Date();
    var r = d.getFullYear() + "-";
    if (d.getMonth() < 10) {
        r += '0';
    }
    r += d.getMonth() + "-";
    if (d.getDate() < 10) {
        r += '0';
    }
    r += d.getDate() + " ";

    if (d.getHours() < 10) {
        r += '0';
    }
    r += d.getHours() + ".";
    if (d.getMinutes() < 10) {
        r += '0';
    }
    r += d.getMinutes();

    return r;
}