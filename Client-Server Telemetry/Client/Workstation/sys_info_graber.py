host="192.168.0.37"
port=8086
user = 'slave'
password = 'batrak'
dbname = 'element'

import wmi
w = wmi.WMI(namespace="root\\wmi")

from influxdb import InfluxDBClient
client = InfluxDBClient(host, port, user, password, dbname)

local_ip = "192.168.0.83"
cpu_percent = psutil.cpu_percent(interval=2, percpu=False);
cpu_temp	= ((w.MSAcpi_ThermalZoneTemperature()[0].CurrentTemperature / 10) - 273.2) ;

entry = [{ 'measurement': 'cpuInfo',
                'tags': {
						#'username': username,
						'eth_ip':	local_ip,	//
						},
                'fields': {
						'load': cpu_percent,	#	   
						'temperature': cpu_temp,#
						''

						} }]
ret = client.write_points(entry)

#print(entry)
#print("write_points() to Influxdb on %s db=%s worked=%s"%(host, dbname, str(ret)))
