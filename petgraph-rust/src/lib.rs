use proxy_wasm::traits::*;
use proxy_wasm::types::*;
use std::time::Duration;

use petgraph::graph::{edge_index};
use petgraph::prelude::*;

use petgraph::algo::{is_isomorphic_matching};

#[no_mangle]
pub fn _start() {
    proxy_wasm::set_log_level(LogLevel::Info);
    proxy_wasm::set_root_context(|_root_context_id| -> Box<dyn RootContext> {
        Box::new(UpstreamCall::new())
    });
    proxy_wasm::set_http_context(|_context_id, _root_context_id| -> Box<dyn HttpContext> {
        Box::new(UpstreamCall::new())
    });
}

#[derive(Debug)]
struct UpstreamCall {}

impl UpstreamCall {
    fn new() -> Self {
        Self {}
    }
}

impl HttpContext for UpstreamCall {
    fn on_http_request_headers(&mut self, _num_headers: usize) -> Action {
        let token = self
            .get_http_request_header("token")
            .unwrap_or(String::from(""));
        proxy_wasm::hostcalls::log(
            LogLevel::Info,
            format!("Auth header : {:?}", token).as_str(),
        );
        let x = self.dispatch_http_call(
            "wasm_upstream",
            vec![
                (":method", "GET"),
                (":path", "/auth"),
                (":authority", "wasm_upstream"),
                ("token", token.as_str()),
            ],
            None,
            vec![],
            Duration::from_secs(5),
        );
        proxy_wasm::hostcalls::log(LogLevel::Info, format!("{:?}", x).as_str());
        Action::Continue
    }

    fn on_http_response_headers(&mut self, _num_headers: usize) -> Action {
        Action::Continue
    }
}

impl Context for UpstreamCall {
    fn on_http_call_response(
        &mut self,
        _token_id: u32,
        _num_headers: usize,
        _body_size: usize,
        _num_trailers: usize,
    ) {
        if let Some(body) = self.get_http_call_response_body(0, _body_size) {
            if let Ok(body) = std::str::from_utf8(&body) {
                proxy_wasm::hostcalls::log(
                    LogLevel::Info,
                    format!("HTTP Call body : {:?} {:?}", body, body == "Authorized").as_str(),
                );
                if body == "Authorized" {
                    self.resume_http_request();
                    return;
                }
                self.send_http_response(
                    403,
                    vec![("Powered-By", "proxy-wasm")],
                    Some(b"Access forbidden.\n"),
                );
            }
        }
    }
}
impl RootContext for UpstreamCall {
    fn on_vm_start(&mut self, _vm_configuration_size: usize) -> bool {
        proxy_wasm::hostcalls::log(LogLevel::Debug, "VM instantiated").unwrap();
        self.set_tick_period(Duration::from_secs(2));
        true
    }

    fn on_tick(&mut self) {
        let g0 = Graph::<(), _>::from_edges(&[(0, 0, 1), (0, 1, 2), (0, 2, 3), (1, 2, 4)]);

        let mut g1 = g0.clone();
        proxy_wasm::hostcalls::log(
            LogLevel::Debug,
            format!(
                "g0 and g1 isomorphic? {}",
                is_isomorphic_matching(&g0, &g1, |x, y| x == y, |x, y| x == y)
            )
            .as_str(),
        )
        .unwrap();

        g1[edge_index(0)] = 0;
        proxy_wasm::hostcalls::log(
            LogLevel::Debug,
            format!(
                "g0 and g1 isomorphic? {}",
                is_isomorphic_matching(&g0, &g1, |x, y| x == y, |x, y| x == y)
            )
            .as_str(),
        )
        .unwrap();

        let mut g2 = g0.clone();
        g2[edge_index(1)] = 0;
        proxy_wasm::hostcalls::log(
            LogLevel::Debug,
            format!(
                "g1 and g2 isomorphic? {}",
                is_isomorphic_matching(&g0, &g2, |x, y| x == y, |x, y| x == y)
            )
            .as_str(),
        )
        .unwrap();
    }
}
