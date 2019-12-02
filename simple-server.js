var express = require('express');
var app = express();
express.static.mime.types['wasm'] = 'application/wasm';
app.use(express.static(__dirname + '/static'));
app.listen(8000, function(err) {
    console.log("WHAT THE EFF", err);
});
