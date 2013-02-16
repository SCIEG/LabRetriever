
function getISODate() {
    var d = new Date();
    var r = d.getFullYear() + "-";
    if (d.getMonth() < 9) {
        r += '0';
    }
    r += (d.getMonth() + 1) + "-";
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