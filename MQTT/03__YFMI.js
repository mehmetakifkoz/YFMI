// MQTT.fx script fountain, modified by Stephen Borsay

var Thread = Java.type("java.lang.Thread");
var topic = "YFMI";
var waitTime = 2000;
var iterations = 10;

function execute(action) {
    out("Test Script: " + action.getName());
    for (var i = 0; i < iterations; i++) {
        sendPayload(i + 1); // Pass sample number as an argument
        Thread.sleep(waitTime);
    }
    action.setExitCode(0);
    action.setResultText("done.");
    out("Test Script: Done");
    return action;
}

function sendPayload(sample) { // Receive sample as an argument
    var temp = parseFloat((Math.random() * (45 - 25) + 25).toFixed(2));
    var ph_mV = parseFloat((Math.random() * (150 - 45) + 45).toFixed(2)); // Assuming the range of PH_mV sensor
    var tds_mV = parseFloat((Math.random() * (150 - 45) + 45).toFixed(2)); // Assuming the range of TDS_mV sensor
    var now = new Date();
    var offsetString = "+03:00";
    var rec_datetime = now.toISOString().replace('Z', offsetString);

    var IoT_Payload = {
        "REC_DATETIME": rec_datetime,
        "SAMPLE": sample,
        "TEMP": temp,
        "PH_mV": ph_mV,
        "TDS_mV": tds_mV
    };

    var payload = JSON.stringify(IoT_Payload);

    mqttManager.publish(topic, payload);
    out("Topic is:  \n" + topic);
    out("payload sent \n" + payload);
}

function out(message) {
    output.print(message);
}
