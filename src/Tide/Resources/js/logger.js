
var file = Ti.Filesystem.getFile(Ti.Filesystem.getApplicationDataDirectory(), "user.properties");
var userProperties;

//if file exists, then load properties.
if(file.exists()) {
    userProperties = Ti.App.loadProperties(file.nativePath());
} else {
//create new set of properties if file doesn't exist
    userProperties = Ti.App.createProperties({
        debug : 'false'
    });
}

userProperties.saveTo(file.nativePath());
file = null;

function log(msg) {
    if (userProperties.getString('debug') == 'true') {
        $("#status").append(msg).append('<br/>');
    }
}