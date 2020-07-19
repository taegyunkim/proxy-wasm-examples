deploy:
	docker-compose up --build --remove-orphans

# shows only the logs related to WASM filter/singleton
deploy-filtered:
	docker-compose up --build --remove-orphans | grep "\[wasm\]\|Starting"

run: deploy

run-filtered: deploy-filtered
