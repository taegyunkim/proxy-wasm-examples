FROM golang:1.14.3 as bd
WORKDIR /github.com/taegyunkim/proxy-wasm-examples/simple-store
ADD . .
RUN GOPROXY=direct GOSUMDB=off go build -a -o /server .
CMD ["/server"]

