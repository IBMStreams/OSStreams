# Open-source Streams (6.x.x.x)

Open-source, Cloud-native version of [IBM Streams](https://www.ibm.com/support/knowledgecenter/SSCRJU_4.3.0/com.ibm.streams.welcome.doc/doc/ibminfospherestreams-introduction.html). Is is designed to natively run
on top of Kubernetes, leveraging the [operator pattern](https://kubernetes.io/docs/concepts/extend-kubernetes/operator/) to manage its internal
resources like _Jobs_ or _Processing Elements_.

## Building Streams

This tutorial assumes that you have a docker registry running on
`localhost:5000` and that your docker namespace is `$USER`. To change either,
use the `DOCKER_REGISTRY` or `DOCKER_NAMESPACE` variables when calling `make`.

### (Optional) Install a local registry

See [the official documentation](https://docs.docker.com/registry/deploying/).

### Building and pushing the builder image
```bash
$ make builder
```
### Building Streams platform and runtime
```bash
$ make
```
### (Optional) Running unit tests
```bash
$ make platform-test
$ make runtime-test
```
### Building and pushing the runtime image
```bash
$ make runtime-image-build
$ make runtime-image-push
```
### Building and pushing the runtime manifest
```bash
$ make runtime-manifest-build-x86
$ make runtime-manifest-push
```
## Running Streams

The wiki contains a page about running Streams applications on Kubernetes.
The folder `src/java/platform/com.ibm.streams.controller/examples` contains some
job examples to get you started.

## License.

Apache 2.0. Please see the `LICENSE` file.
