# Project Overview: ```secure_mqtt_client_publish_data```

## Changes on ```mqtt_client_publish_data``` Application (Template)

### mqtt_app.c Algorithm

* Waits for Wi-Fi + SNTP time sync
* Connects to a public broker over MQTTS (TLS) on port ```8883```,
* Uses Espressif’s certificate bundle (esp_crt_bundle_attach) for server verification,
* Supports automatic reconnect and will log certificate/time problems,
* Includes guidance for build/configuration and troubleshooting.

### wifi_manager.c

* Enable SNTP/time sync in wifi_manager.c — set ```USE_TLS_CERTIFICATE_BUNDLE``` to 1. TLS certificate verification requires accurate system time.
* Enable the ```mbedTLS certificate``` bundle in menuconfig (if not already): ```idf.py``` menuconfig → Component config → mbedTLS → Enable "Certificate bundle" (CONFIG_MBEDTLS_CERTIFICATE_BUNDLE).

If the ESP-IDF version exposes options like CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_FULL, you can enable that too. This makes esp_crt_bundle_attach() available and provides the default CA store.

### Troubleshooting

1. **Time not synced** → TLS certificate verification fails (mbedtls returns `CERT_VERIFY_FAILED`). Ensure SNTP finished. Increase wait time if needed.

2. **Certificate bundle not included** → `esp_crt_bundle_attach` unresolved or runtime cert verify fails. Enable certificate bundle in `menuconfig` or embed a specific root CA PEM and set `.cert_pem` instead.

3. **Hostname mismatch** → ensure broker hostname in URI matches the server certificate CN/SAN (we use `broker.hivemq.com` — public broker, should match).

4. **Network firewalls** → port 8883 might be blocked on some networks. Verify by testing from PC:

   ```bash
   openssl s_client -connect broker.hivemq.com:8883 -showcerts
   ```

   This confirms server reachability and shows cert chain.

5. **MQTT client configured incorrectly** → ensure `.transport = MQTT_TRANSPORT_OVER_SSL` and `.broker.address.uri` uses `mqtts://...:8883`.

6. **Debugging TLS internals** → enable mbedTLS debug logs in `menuconfig` for deeper handshake information.

---
