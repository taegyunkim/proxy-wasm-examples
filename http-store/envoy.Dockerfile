FROM istio/proxyv2:1.5.4
ENTRYPOINT /usr/local/bin/envoy -c /etc/envoy.yaml -l debug --service-cluster proxy

