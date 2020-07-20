## Singleton-HTTP-Call

WASM service calls an upstream service every 5 seconds. Check the logs for
the response of the request.

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
bazel build filter.wasm
make deploy-filtered
```

