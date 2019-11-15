// 注册压测
if (process.argv.length < 4) {
    console.log('请输入每批处理的人数和总共要处理的批次数量!!!');
    return;
}

function getDateTime() {
    var dateTime = new Date();
    return dateTime.toLocaleDateString() + "-" + dateTime.getHours();
};

var request = require('request');
var b64 = require('../nodejs-test/base64_encode.js');
const readline = require('readline');
const fs = require('fs');
var address = "http://127.0.0.1:33388";
var imageDir = "/home/lchy/dataSet/casia-10000";
var imageFileList = "reg-imagelist.txt";
var timeout = 2000;
var timeid = null;
var total = 0;
var totalResponseTime = 0;
var totalProcessTime = 0;
var maxResponseTime = 0;
var minResponseTime = 1000000;
var maxProcessTime = 0;
var minProcessTime = 1000000;
var tps = process.argv[2]
var testCount = process.argv[3];
var outputFile = getDateTime() + "-" + "reg-" + tps + "-" + testCount + ".log";

if (process.argv.length == 5)
    address = "http://" + process.argv[4] + ":33388";
else if (process.argv.length == 6)
    address = "http://" + process.argv[4] + ":" + process.argv[5];


var data = {
    api_key: '',
    image_base64: [],
    face_id: '000001',
    group_id: '000001'
};

var options = {
    url: address + '/reg',
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(data),
    time: true
};

function callback(err, httpResponse, body) {
    if (err) { console.log(err.name + ": " + err.message); return; }
    var res = JSON.parse(body);
    var responseMsg = "response_time: " + httpResponse.timings.response.toFixed(2) + " | process_time: " + res.time_used + " | error_message: " + res.error_message;
    fs.appendFileSync(outputFile, responseMsg + '\n', 'utf-8');
    totalResponseTime += httpResponse.timings.response;
    totalProcessTime += res.time_used;
    if (httpResponse.timings.response > maxResponseTime) maxResponseTime = httpResponse.timings.response;
    if (httpResponse.timings.response < minResponseTime) minResponseTime = httpResponse.timings.response;
    if (res.time_used > maxProcessTime) maxProcessTime = res.time_used;
    if (res.time_used < minProcessTime) minProcessTime = res.time_used;
    console.log(responseMsg);
};

var photos = [];
{
    var text = fs.readFileSync(imageDir + '/' + imageFileList, 'utf8');
    text.split(/\r?\n/).forEach(function (line) {
        photos.push(line);
    });
}

function handleRegister() {
    if (total >= testCount || (total + 1) * tps >= photos.length) {
        clearInterval(timeid);
        var result = '-----------------------API:reg 测试结果-----------------------\n';
        result += '并发数      : ' + tps + '\n';
        result += '测试批数    : ' + testCount + '\n';
        result += '最大响应时间: ' + maxResponseTime.toFixed(2) + '\n';
        result += '最小响应时间: ' + minResponseTime.toFixed(2) + '\n';
        result += '最大处理时间: ' + maxProcessTime.toFixed(2) + '\n';
        result += '最小处理时间: ' + minProcessTime.toFixed(2) + '\n';
        result += '平均响应时间: ' + (totalResponseTime / (tps * total)).toFixed(2) + '\n';
        result += '平均处理时间: ' + (totalProcessTime / (tps * total)).toFixed(2) + '\n';
        fs.appendFileSync(outputFile, result, 'utf-8');
        console.log(result);
        return;
    }

    var turn = true;
    var faceId = "000001";
    var faces = new Array();

    for (var i = 0; i <= tps; ++i) {
        var line = photos[total * tps + i]
        //读图片编码
        faces[0] = b64.base64_encode(imageDir + '/casia/' + line);
        faceId = line.split("/")[0];

        data.image_base64 = faces;
        data.face_id = faceId;
        options.body = JSON.stringify(data);

        if (i % 10 == 0) 
        {
            turn = !turn;
            if (turn) options.url = "http://127.0.0.1:33388/reg";
            else options.url = "http://127.0.0.1:33389/reg";
        }
        request.post(options, callback);
    }
    total++;
}

fs.unlink(outputFile, function (err) {
    // if (err) {
    //     return console.error(err);
    // }
});
timeid = setInterval(handleRegister, timeout);
