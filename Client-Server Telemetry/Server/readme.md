# OrangePi PC Server Hot Spot Server ( InfluxDB + Grafana)
## Запись системы на SD-card ##
Скачать Armbian Bionic и записать образ на SD-карту с помощью Win32 Disk imager
## Ethernet ## 
  ```
  nano /etc/network/interfaces
  auto eth0
      iface eth0 inet static
          address 192.168.0.37
          netmask 255.255.255.0
          gateway 192.168.0.1
   ```
## Proxy ##

* Добавляем в файл `/etc/apt/apt.conf`
  ```
  Acquire::http::proxy "http://192.168.0.99:8082/";
  Acquire::https::proxy "https://192.168.0.99:8082/";
  Acquire::ftp::proxy "ftp://192.168.0.99:8082/";
  ```
* Добавляем в файл `nano /etc/environment`
  ```
  http_proxy="http://192.168.0.99:8082/"
  https_proxy="https://192.168.0.99:8082/"
  ftp_proxy="ftp://192.168.0.99:8082/"
  no_proxy="localhost, 127.0.0.0/8, 192.168.0.0/16”
  ```
## Время и дата ## 
  1. Настроим дату и время
    ```
    sudo date -s "2019-03-12 15:12:22"
    ```
  2. Создадим скрипт автообновления времени `sudo nano /etc/cron.daily/httpdate`
      ```
      #!/bin/bash

      HOST="192.168.0.99"
      PORT="8082"
      CHECKHOST="google.com" # Хост для проверки доступности сети
      DATEHOST="http://google.com" # Хост с которого будем брать время

      PROXY="http://"$HOST":"$PORT

      if ( ! ping -c5 -i1 -n -s10 -W1 $HOST  &>/dev/null ); then
          logger "HTTPDate: Proxy server in unreachable."
          exit 1
      fi

      date -s "$(wget -e use_proxy=yes -e http_proxy="$PROXY" -S - "$DATEHOST" 2>&1 |
              grep -E '^[[:space:]]*[dD]ate:' |
              sed 's/^[[:space:]]*[dD]ate:[[:space:]]*//' |
              head -1l | awk '{print $1, $3, $2,  $5 ,"GMT", $4 }' |
              sed 's/,//')" &>/dev/null
      logger "Success date update: "$(date -u)
      ```
  3. Добавляем скрипт в автозапуск `nano /etc/rc.local`
      ```
      #!/bin/sh -e source ./venv/bin/activate
      /etc/cron.daily/httpdate
      exit 0
      ```
  4. Отключаем службу NTP:
    ```
    sudo systemctl stop ntp.service
    sudo systemctl disable ntp.service
    ``` 
## Обновить систему ##
```
sudo apt-get update && upgrade
```
## Установить InfluxDB ##

https://portal.influxdata.com/downloads/
или
`apt-get install influxdb`
после установки запускаем influx и создаем БД: `create database esp`

## Установить Grafana ##
http://docs.grafana.org/installation/debian/
входим по admin/admin
добавляем дадасурс
дашборд и лепим на него что угодно

## Настроить OrangePi HotSpot (WiFi Access Point) + автозапуск ##	
```
sudo systemctl enable create_ap.service
```
## Если была установлена десктопная версия, то следует отключить автозапуск GUI ##
```
sudo systemctl disable nodm
sudo reboot
```
