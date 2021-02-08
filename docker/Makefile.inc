.PHONY: build push
default: build push

build: pre-build
	@docker build \
		--build-arg DOCKER_ARCH=$(DOCKER_ARCH) \
		--build-arg DOCKER_IMAGE=$(DOCKER_IMAGE) \
		--build-arg DOCKER_NAMESPACE=$(DOCKER_NAMESPACE) \
		--build-arg DOCKER_REGISTRY=$(DOCKER_REGISTRY) \
		--build-arg NPROC=$(NPROC) \
		-t $(DOCKER_REGISTRY)/$(DOCKER_NAMESPACE)/$(NAME):$(VERSION) $(WORKDIR)

push: pre-push
	@docker push $(DOCKER_REGISTRY)/$(DOCKER_NAMESPACE)/$(NAME):$(VERSION)
