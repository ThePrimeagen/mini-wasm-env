var express = require('express');
var app = express();
express.static.mime.types['wasm'] = 'application/wasm';
app.use(express.static(__dirname + '/static'));
port=8000
app.listen(8000, function(err) {
    console.log("WHAT THE EFF", err);
});
