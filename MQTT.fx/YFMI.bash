#!/bin/bash

# Set the AWS IoT endpoint
ENDPOINT="<INSERT YOUR ENDPOINT HERE>"

# Set the paths to your certificate files
CACERT="AmazonRootCA1.pem"
CERT="client.pem.crt"
KEY="private.pem.key"

# Set the MQTT topic and QoS level
TOPIC="YFMI"
QOS=1

# Loop from 1 to 10
for ((sample=1; sample<=10; sample++)); do
    # Set the data for the payload
    REC_DATETIME=$(date +"%Y-%m-%dT%H:%M:%S%:z")
    AT=$(LC_NUMERIC=C printf "%.2f" $(echo "scale=2; ($RANDOM / 32768) * (45 - 25) + 25" | bc))
    DT1=$(LC_NUMERIC=C printf "%.2f" $(echo "scale=2; ($RANDOM / 32768) * (45 - 25) + 25" | bc))
    DT2=$(LC_NUMERIC=C printf "%.2f" $(echo "scale=2; ($RANDOM / 32768) * (45 - 25) + 25" | bc))
    PH_mV=$(LC_NUMERIC=C printf "%.2f" $(echo "scale=2; ($RANDOM / 32768) * (150 - 45) + 45" | bc))
    TDS_mV=$(LC_NUMERIC=C printf "%.2f" $(echo "scale=2; ($RANDOM / 32768) * (150 - 45) + 45" | bc))

    # Construct the payload
    PAYLOAD="{\"REC_DATETIME\":\"$REC_DATETIME\",\"SAMPLE\":$sample,\"AT\":$AT,\"DT1\":$DT1,\"DT2\":$DT2,\"PH_mV\":$PH_mV,\"TDS_mV\":$TDS_mV}"

    # Send the payload using curl
    curl --tlsv1.2 \
        --cacert $CACERT \
        --cert $CERT \
        --key $KEY \
        --request POST \
        --data "$PAYLOAD" \
        "https://$ENDPOINT:8443/topics/$TOPIC?qos=$QOS"

    # Wait for a while before sending the next payload
    sleep 1
done
