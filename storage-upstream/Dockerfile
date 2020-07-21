FROM golang:1.14.3 as bd
WORKDIR /github.com/taegyunkim/proxy-wasm-examples/storage-upstream
ADD . .
RUN GOPROXY=direct GOSUMDB=off go build -a -o /server .
CMD ["/server"]

