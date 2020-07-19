## TCP-Metrics

Collects simple metrics for every TCP connection and logs it.

Build filter:

```shell
bazel build filter.wasm
```

Build upstream service (needs to be done once):

```shell
make build-upstream
```

Deploy:

```bash
make deploy-filtered
```

Test: Check the logs for the metrics.

```bash
curl 0.0.0.0:18000 -v -d "request body"
```
