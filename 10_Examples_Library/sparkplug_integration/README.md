# Sparkplug Integration Examples

This directory contains examples demonstrating how to use NDH's Sparkplug connector.

## Overview

Sparkplug is an MQTT-based specification for Industrial IoT (IIoT) that provides:
- Standardized topic namespace
- Standardized payload format (Protocol Buffers)
- State management (BIRTH/DEATH messages)
- Automatic device discovery

## Prerequisites

### 1. MQTT Broker

You need an MQTT broker that supports MQTT 3.1.1. We recommend:

**Option A: Eclipse Mosquitto (Docker)**

```bash
docker run -d \
  --name mosquitto \
  -p 1883:1883 \
  eclipse-mosquitto \
  mosquitto -c /mosquitto-no-auth.conf
```

**Option B: HiveMQ Community Edition (Docker)**

```bash
docker run -d \
  --name hivemq \
  -p 1883:1883 \
  hivemq/hivemq-ce
```

### 2. Install Dependencies

```bash
pip install paho-mqtt loguru
```

Optional (for Kafka integration):

```bash
pip install aiokafka
```

## Examples

### 1. Basic Sparkplug Subscriber

**File**: `basic_sparkplug_subscriber.py`

This example demonstrates how to:
- Connect to an MQTT broker
- Subscribe to Sparkplug topics
- Receive and process Sparkplug events
- Display statistics

**Usage**:

```bash
python basic_sparkplug_subscriber.py
```

**Output**:

```
============================================================
NDH Sparkplug Subscriber Example
============================================================

Configuration:
  Broker: localhost:1883
  Group ID: #
  QoS: 1

Connecting to MQTT Broker...
Connected successfully!

Waiting for Sparkplug messages...
Press Ctrl+C to stop
------------------------------------------------------------

📨 Event: asset.created
   Asset: Factory1_Gateway1_Sensor001
   Time: 2025-10-10T12:34:56.789
   ✅ New device online!
   Metrics: ['temperature', 'humidity', 'pressure']

📨 Event: asset.data_changed
   Asset: Factory1_Gateway1_Sensor001
   Time: 2025-10-10T12:35:01.234
   📊 Data updated
   Metrics: {'temperature': 25.5, 'humidity': 60.2}
```

### 2. Sparkplug to Kafka Bridge

**File**: `sparkplug_to_kafka.py`

This example demonstrates how to:
- Create a bridge between Sparkplug MQTT and Kafka
- Publish Sparkplug events to Kafka topics
- Build a complete data flow from edge to enterprise

**Architecture**:

```
Sparkplug Devices → MQTT Broker → Sparkplug Connector → Kafka → Enterprise Systems
```

**Kafka Topics**:
- `ndh.assets.created` - Device birth events
- `ndh.assets.data_changed` - Data update events
- `ndh.assets.status_changed` - Device death events

**Usage**:

```bash
# Start Kafka (if not already running)
docker-compose up -d kafka

# Run the bridge
python sparkplug_to_kafka.py
```

**Output**:

```
============================================================
Sparkplug to Kafka Bridge Example
============================================================

Configuration:
  MQTT Broker: localhost:1883
  Kafka: localhost:9092
  Group ID: #

Starting Sparkplug to Kafka bridge...
Connecting to MQTT Broker...
Creating Kafka producer...
Bridge started successfully!

Bridge is running...
Sparkplug events will be published to Kafka topics:
  - asset.created → ndh.assets.created
  - asset.data_changed → ndh.assets.data_changed
  - asset.status_changed → ndh.assets.status_changed

Press Ctrl+C to stop
------------------------------------------------------------

📊 Statistics:
   Sparkplug Messages: 150
   Births: 5
   Deaths: 0
   Data: 145
   Edge Nodes: 2 (2 online)
   Devices: 5 (5 online)
   Kafka Events Published: 150
   Kafka Events Failed: 0
```

## Testing with Simulated Devices

If you don't have real Sparkplug devices, you can use the following tools to simulate them:

### Option 1: MQTT.fx (GUI Tool)

1. Download MQTT.fx from https://mqttfx.jensd.de/
2. Connect to your MQTT broker
3. Publish Sparkplug messages manually

**Example DBIRTH message**:

Topic: `spBv1.0/Factory1/DBIRTH/Gateway1/Sensor001`

Payload (JSON for simplicity):
```json
{
  "timestamp": 1696934400000,
  "metrics": {
    "temperature": 25.5,
    "humidity": 60.2,
    "pressure": 1013.25
  }
}
```

### Option 2: Python Script

Create a simple Sparkplug publisher:

```python
import paho.mqtt.client as mqtt
import json
import time

client = mqtt.Client()
client.connect("localhost", 1883)

# Publish DBIRTH
topic = "spBv1.0/Factory1/DBIRTH/Gateway1/Sensor001"
payload = {
    "timestamp": int(time.time() * 1000),
    "metrics": {
        "temperature": 25.5,
        "humidity": 60.2,
        "pressure": 1013.25
    }
}
client.publish(topic, json.dumps(payload))

# Publish DDATA every 5 seconds
while True:
    topic = "spBv1.0/Factory1/DDATA/Gateway1/Sensor001"
    payload = {
        "timestamp": int(time.time() * 1000),
        "metrics": {
            "temperature": 25.5 + (time.time() % 10),
            "humidity": 60.2 + (time.time() % 5)
        }
    }
    client.publish(topic, json.dumps(payload))
    time.sleep(5)
```

## Sparkplug Topic Structure

Sparkplug uses a standardized topic structure:

```
spBv1.0/{group_id}/{message_type}/{edge_node_id}/{device_id}
```

**Examples**:
- `spBv1.0/Factory1/DBIRTH/Gateway1/Sensor001` - Device birth
- `spBv1.0/Factory1/DDATA/Gateway1/Sensor001` - Device data
- `spBv1.0/Factory1/DDEATH/Gateway1/Sensor001` - Device death
- `spBv1.0/Factory1/NBIRTH/Gateway1` - Edge node birth
- `spBv1.0/Factory1/NDATA/Gateway1` - Edge node data
- `spBv1.0/Factory1/NDEATH/Gateway1` - Edge node death

## Message Types

| Type | Description | Direction |
|------|-------------|-----------|
| **NBIRTH** | Edge Node Birth | Device → Broker |
| **NDEATH** | Edge Node Death | Device → Broker |
| **DBIRTH** | Device Birth | Device → Broker |
| **DDEATH** | Device Death | Device → Broker |
| **NDATA** | Edge Node Data | Device → Broker |
| **DDATA** | Device Data | Device → Broker |
| **NCMD** | Edge Node Command | Broker → Device |
| **DCMD** | Device Command | Broker → Device |
| **STATE** | SCADA Host State | SCADA → Broker |

## Troubleshooting

### Connection Failed

**Problem**: `Failed to connect to MQTT Broker`

**Solution**:
1. Check if MQTT broker is running: `docker ps`
2. Check if port 1883 is accessible: `telnet localhost 1883`
3. Check broker logs: `docker logs mosquitto`

### No Messages Received

**Problem**: Connector is connected but no messages are received

**Solution**:
1. Check if devices are publishing: Use MQTT.fx to subscribe to `spBv1.0/#`
2. Check topic format: Ensure topics follow Sparkplug format
3. Check QoS level: Try QoS 0 or 1

### Kafka Connection Failed

**Problem**: `Failed to connect to Kafka`

**Solution**:
1. Check if Kafka is running: `docker ps`
2. Check if port 9092 is accessible: `telnet localhost 9092`
3. Check Kafka logs: `docker logs kafka`

## Next Steps

1. Read the [Sparkplug Integration Guide](../../docs/SPARKPLUG_INTEGRATION.md)
2. Read the [Sparkplug Implementation Plan](../../docs/SPARKPLUG_IMPLEMENTATION_PLAN.md)
3. Explore the [Sparkplug connector source code](../../ndh/connectors/sparkplug_connector.py)

## References

- [Sparkplug Specification](https://sparkplug.eclipse.org/specification/)
- [Eclipse Tahu (Reference Implementation)](https://github.com/eclipse-tahu/tahu)
- [HiveMQ Sparkplug Essentials](https://www.hivemq.com/blog/mqtt-sparkplug-essentials-part-1-introduction/)

---

**Author**: Chih-Cheng Lin (Michael Lin)  
**Date**: 2025-10-10  
**Version**: 1.0

