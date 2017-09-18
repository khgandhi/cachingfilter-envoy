# Ebay Envoy

This repo contains eBay's L7 Proxy ([envoy proxy](https://github.com/envoyproxy/envoy)) with an caching filter to add 
support for an external caching layer to envoy.

# Caching Engine

The external caching engine uses [Apache Traffic Server](http://trafficserver.apache.org/) as the caching layer. The Engine also integrates with an external Orchestrator service that is designed to bring in eBay services like Expermintation, Server Side Tracking into the caching layer.

# Repo
This repo is configured to use the master envoy proxy code as a submodule to the caching filter.

# Build
The caching filter will be statically compiled with the envoy proxy code. To build envoy, we can use the envoy-build docker image:

``` shell
$ docker run -it --name envoy-build lyft/envoy-build:b3542cdfe76e7e46c276c781935d339d4ecd0502 /bin/bash
```

Once the container is up and running, we can clone this repo inside the container:

```shell

```



