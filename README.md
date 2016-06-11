
Currently this is a filter. If it really works and if people want it, we can add subscribe / publishing stuff to this. Currently run:

```
mosquitto_sub -v -t owa/cs/# | ./owa11-owntracks | mosquitto_pub -t owntracks/username/owa11 -l
```

In other words:

* subscribe to the owa11 publishes
* run the payloads (yes, leading topic is fine) through the owa11-owntracks filter
* pipe the result to stdin (`-l`) of a mosquitto_pub to publish back into Recorder
