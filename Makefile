help:
	@echo "For local version run 'make local'"
	@echo "    This command will build project locally in ./src directory"
	@echo
	@echo "For docker version run 'make docker'"
	@echo "    This command will build docker image with name 'matbur_image'"
	@echo "    and container with name 'matbur_container'"
	@echo "    to delete both run 'make docker_clean'"

local:
	make -C src -j
	@echo;
    ./src/so_pro;
	@echo;
    @echo "usage: ./src/so_pro -i FILE -n NUM";
    @echo "example: ./src/so_pro -i urls.txt -n 4"

local_clean:
	make -C src clean

docker: docker_build docker_run

docker_build:
	docker build -t matbur_image:latest .

docker_run:
	docker run -it --name matbur_container matbur_image:latest

docker_clean: docker_rm docker_rmi

docker_rm:
	docker rm matbur_container

docker_rmi:
	docker rmi matbur_image
