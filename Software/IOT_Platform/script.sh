#!/bin/bash

while :
do
	
	curl -v -X POST -d "{"payload":[$[ $RANDOM % 30 + 10 ],0,0,0,0,1,30]}" https://demo.thingsboard.io/api/v1/T4heef9dzbcuejT5qegC/telemetry --header "Content-Type:application/json"
	
	sleep 600
done

 curl -v -X POST -d "{"payload":[1,0,0,0,0,1,0]}" http://demo.thingsboard.io/api/v1/T4heef9dzbcuejT5qegC/telemetry --header "Content-Type:application/json"



 curl -v -X POST -d "{"payload":[0,0,0,2,0,1,60]}" http://demo.thingsboard.io/api/v1/T4heef9dzbcuejT5qegC/telemetry --header "Content-Type:application/json"