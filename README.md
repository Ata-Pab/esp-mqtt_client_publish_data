# Project Overview: ```mqtt_client_publish_data```

## Public Broker (Unsecured)

We'll use **HiveMQ’s public broker**, which is very stable and widely used for development:

* **Broker URL:** `mqtt://broker.hivemq.com`
* **Port:** `1883` (non-TLS)
* **Topic for test:** `esp32/test`

This broker doesn’t require authentication.

## MQTT Flow for this ESP32 Application

```
   +-----------------------------------------------+
   |                  ESP32 Device                 |
   +-----------------------------------------------+
   |                                               |
   |   1. Connect to Wi-Fi                         |
   |   2️. Connect to MQTT broker                   |
   |   3️. Subscribe to topic "esp32/test"          |
   |   4️. Every 5 seconds → Publish message        |
   |       {"device":"esp32","counter":N}          |
   |   5️. Log received messages via callback       |
   |                                               |
   +-----------------------------------------------+
```

This application will be **FreeRTOS-friendly**:

* One task for publishing messages.
* MQTT event callbacks for connection/subscription handling.

## Project Structure

```
/main
 ├── main.c              // app entry, starts wifi + mqtt
 ├── mqtt_app.c/.h       // mqtt logic (publish/subscribe)
 ├── wifi_manager.c/.h   // same as before
 └── CMakeLists.txt
```

## Core MQTT Concepts

| Concept                      | Description                                                   |
| ---------------------------- | ------------------------------------------------------------- |
| **Broker**                   | The MQTT server that routes messages between clients.         |
| **Topic**                    | A string like `"home/sensor/temp"` used for message grouping. |
| **Publish**                  | Send a message to a topic.                                    |
| **Subscribe**                | Receive all messages published to a topic.                    |
| **QoS (Quality of Service)** | Message delivery guarantee level (0, 1, or 2).                |
| **Retained Message**         | Broker keeps the last message on a topic for new subscribers. |

---
