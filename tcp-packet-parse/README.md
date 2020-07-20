## TCP packet parse

Not sure what was the original intent. It simply captures downstream request body and logs. 

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

