# Prometheus Support

Adding an endpoint at:

```
/metrics
```

Responding to queries with:

```
arduino_temperature_probe 20.3
```

http://192.168.2.70/metric

```
arduino_temperature_probe 20.00
```

Config for `prometheus.yml`:

```
  - job_name: 'External Ambient Temperature Monitor'
    scrape_interval: 5s
    static_configs:
      - targets: ['192.168.2.70:80']
```

