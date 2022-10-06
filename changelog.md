# AsyncWebServer_Ethernet

[![arduino-library-badge](https://www.ardu-badge.com/badge/AsyncWebServer_Ethernet.svg?)](https://www.ardu-badge.com/AsyncWebServer_Ethernet)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/AsyncWebServer_Ethernet.svg)](https://github.com/khoih-prog/AsyncWebServer_Ethernet/releases)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/AsyncWebServer_Ethernet.svg)](http://github.com/khoih-prog/AsyncWebServer_Ethernet/issues)

---
---

## Table of contents

* [Changelog](#changelog)
  * [Releases v1.5.0](#releases-v150)
  * [Releases v1.4.1](#releases-v141)

---
---

## Changelog

#### Releases v1.5.0

1. Support using `CString` to save heap to send `very large data`. Check [request->send(200, textPlainStr, jsonChartDataCharStr); - Without using String Class - to save heap #8](https://github.com/khoih-prog/Portenta_H7_AsyncWebServer/pull/8) and [All memmove() removed - string no longer destroyed #11](https://github.com/khoih-prog/Portenta_H7_AsyncWebServer/pull/11)
2. Add multiple examples to demo the new feature

#### Releases v1.4.1

1. Initial coding to port [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) to ESP8266 boards using W5x00 / ENC28J60 Ethernet.
2. Add more examples.
3. Add debugging features.
4. Bump up to v1.4.1 to sync with [AsyncWebServer_WT32_ETH01 v1.4.1](https://github.com/khoih-prog/AsyncWebServer_WT32_ETH01).


