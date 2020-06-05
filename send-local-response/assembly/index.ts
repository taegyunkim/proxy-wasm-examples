export * from "@solo-io/proxy-runtime/proxy";
import {
  RootContext,
  Context,
  RootContextHelper,
  ContextHelper,
  registerRootContext,
  FilterHeadersStatusValues,
  stream_context,
} from "@solo-io/proxy-runtime";
import {
  send_local_response,
  GrpcStatusValues,
} from "@solo-io/proxy-runtime/runtime";

class AddHeaderRoot extends RootContext {
  configuration: string;

  onConfigure(): bool {
    let conf_buffer = super.getConfiguration();
    let result = String.UTF8.decode(conf_buffer);
    this.configuration = result;
    return true;
  }

  createContext(): Context {
    return ContextHelper.wrap(new AddHeader(this));
  }
}

function stringToArrayBuffer(str: string): ArrayBuffer {
  var arr = new Uint8Array(str.length);
  for (var i = str.length; i--; ) arr[i] = str.charCodeAt(i);
  return arr.buffer;
}

class AddHeader extends Context {
  root_context: AddHeaderRoot;
  constructor(root_context: AddHeaderRoot) {
    super();
    this.root_context = root_context;
  }
  onRequestHeaders(a: u32): FilterHeadersStatusValues {
    send_local_response(
      200,
      "success",
      stringToArrayBuffer("hello, world"),
      [],
      GrpcStatusValues.Ok
    );

    return FilterHeadersStatusValues.StopIteration;
  }
  onResponseHeaders(a: u32): FilterHeadersStatusValues {
    stream_context.headers.response.add("hello", "world!");
    return FilterHeadersStatusValues.Continue;
  }
}

registerRootContext(() => {
  return RootContextHelper.wrap(new AddHeaderRoot());
}, "add_header");
