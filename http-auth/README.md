## HTTP-Auth

Simulates handling authentication of requests at proxy level. Requests with a
header `token` with value `hello` are accepted as authorized while the rest
unauthorized. The actual authentication is handled by the Upstream server.
Whenever the proxy recieves a request it extracts the `token` header and makes
a request to the Upstream server which validates the token and returns a
response.

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

Test:

```bash
curl  -H "token":"hello" 0.0.0.0:18000 -v # Authorized
curl  -H "token":"world" 0.0.0.0:18000 -v # Unauthorized
```

Deploying to Bookinfo demo application

- Follow [Getting
  Started](https://archive.istio.io/v1.5/docs/setup/getting-started/) to install
  Bookinfo application with Istio 1.5.x
- Start wasm_upstream service

```shell
kubectl apply -f ./wasm_upstream.yaml
```

- Add wasm_upstream cluster to productpage service

```shell
kubectl delete -f ./productpage-cluster.yaml
```

- Build, push, deploy with [wasme cli](https://github.com/solo-io/wasme),
  assumming that you have already run `bazel build filter.wasm`

```shell
wasme build precompiled ./bazel-bin/filter.wasm \
  --tag webassemblyhub.io/<your webassemblyhub user id>/<filter name>:<tag> \
  --config runtime-config.json

wasme push webassemblyhub.io/<your webassemblyhub user id>/<filter name>:<tag>

wasme deploy istio webassemblyhub.io/<your webassemblyhub user id>/<filter name>:<tag> \
  --id=auth --namespace=default --labels=app=productpage
```

I have already built and pushed the filter to WebAssembly Hub, so run following
if you didn't make any changes to the filter.

```shell
wasme deploy istio webassemblyhub.io/taegyunk/http-auth:v0.1 \
  --id=auth --namespace=default --labels=app=productpage
```
