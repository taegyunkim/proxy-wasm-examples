build:
	bazel build filter.wasm

deploy:
	docker-compose up --build --remove-orphans

# shows only the logs related to WASM filter/singleton
deploy-filtered:
	docker-compose up --build --remove-orphans | grep "\[wasm\]\|Starting"

run: build deploy

run-filtered: build deploy-filtered

clean:
	bazel clean

