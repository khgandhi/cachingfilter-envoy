docker build -f dockerfile-ebay-envoy -t UFESDev/ebay-envoy:latest .
docker login -u "$DOCKERHUB_USERNAME" -p "$DOCKERHUB_PASSWORD"
docker push UFESDev/ebay-envoy:latest
docker tag UFESDev/ebay-envoy:latest UFESDev/envoy:"$EBAY_ENVOY_SHA1"
docker push UFESDev/ebay-envoy:"$EBAY_ENVOY_SHA1"

docker build -f dockerfile-ebay-envoy-alpine -t UFESDev/ebay-envoy-alpine:latest .
docker tag UFESDev/ebay-envoy-alpine:latest UFESDev/envoy-alpine:"$EBAY_ENVOY_SHA1"
docker push UFESDev/ebay-envoy-alpine:"$EBAY_ENVOY_SHA1"
docker push UFESDev/ebay-envoy-alpine:latest

docker build -f dockerfile-ebay-envoy-alpine-debug -t UFESDev/ebay-envoy-alpine-debug:latest .
docker tag UFESDev/ebay-envoy-alpine-debug:latest UFESDev/envoy-alpine-debug:"$EBAY_ENVOY_SHA1"
docker push UFESDev/ebay-envoy-alpine-debug:"$EBAY_ENVOY_SHA1"
docker push UFESDev/ebay-envoy-alpine-debug:latest
