help:
	@echo "For local version run: 'make local'"
	@echo "For docker version run: 'make docker'"

local:
	cd src
	make
	./so_pro

docker: docker_build docker_run

docker_build:
	docker build -f docker/Dockerfile -t mateusz_burniak_image:latest .

docker_run:
	docker run -it --name mateusz_burniak_container mateusz_burniak_image:latest

docker_clean: docker_rm docker_rmi

docker_rm:
	docker rm mateusz_burniak_container

docker_rmi:
	docker rmi mateusz_burniak_image
