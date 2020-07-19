## HTTP-Auth

Simulates handling authentication of requests at proxy level. Requests with a header `token` with value `hello` are accepted as authorized while the rest unauthorized. The actual authentication is handled by the Upstream server. Whenever the proxy recieves a request it extracts the `token` header and makes a request to the Upstream server which validates the token and returns a response.

Deploy:

```bash
cd http-auth
make deploy-filtered
```

Test:

```bash
curl  -H "token":"hello" 0.0.0.0:18000 -v # Authorized
curl  -H "token":"world" 0.0.0.0:18000 -v # Unauthorized
```
