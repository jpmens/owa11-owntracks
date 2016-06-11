
```
mosquitto_sub -v -t owa/cs/# | ./owa11-owntracks | mosquitto_pub -t owntracks/username/owa11 -l
```
