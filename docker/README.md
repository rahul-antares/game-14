# docker

All the following commands are supposed to be run from within the docker folder,

## Build and Start containers

1. Setup required `.env`

    ```sh 
    cp env.sample .env
    ```
    - **Mac Host:** `HOST=localhost` is default, no need to make any changes to `.env`
    - **Linux VM Host:** `HOST=<hostname>.local` if `avahi-daemon` is running (recommended). Unset `GATEWAY_HOST=`

2. Create certs and root users (one-time setup)
   
    ```sh 
    docker compose run --build setup-sysinit
    ```
    
3. Run all the services

    ```sh 
    docker compose up --build  # with rebuilding image
    ```
    OR
    ```sh
    docker compose up # without building image
    ```
    OR
    ```sh
    docker compose -f dev.yml up --build # dev mode with rebuilding image
    ```
    OR
    ```sh
    docker compose -f dev.yml up # dev mode without building image
    ```
5. Access Admin dashboard and FLT Operator

    Open the following links in your browser and use the default user credentials as specific in your `.env`
    
    **Mac Host:**
    - `https://localhost`: Admin dashboard.
    - `https://localhost/op`: FLT Operator
    
    **Linux VM Host:**
    - `https://<hostname>.local`: Admin dashboard.
    - `https://<hostname>.local/op`: FLT Operator

## Stop running containers
```sh
docker compose down
```

## Tail container logs
Lookup for `CONTAINER_NAME` either from `docker-compose.yml` file or running `docker ps`
```
docker logs -f {CONTAINER_NAME}
```

## Tail container logs with Bunyan
In the case of a service that uses Bunyan logger
```sh
npm install -g bunyan
```
```sh
docker logs --no-log-prefix -f {CONTAINER_NAME} | bunyan
```
## Tail container logs with pino-pretty
In the case of a service that uses Pino logger
```sh
npm install -g pino-pretty
```
```sh
docker logs -f {CONTAINER_NAME} | pino-pretty

```
![rgs-docker drawio](https://github.com/forestlaketech.com/rgs/assets/157005284/c88bc560-0824-434c-9210-ace4c38241f9)

## Generate BASE64 encoded RSA Private and Public PEM content

```
# create RSA Private PEM

export $PASS=<JST_PASSPHRASE>
openssl genrsa -passout pass:"$PASS" -des3 -out ./pvt.pem
cat ./pvt.pem | base64

# create RSA Public PEM from Private PEM

openssl rsa -passin pass:"$PASS" -in ./pvt.pem -outform PEM -pubout -out pub.pem
cat ./pub.pem | base64

# Copy these base64 encoded strings to your .env variables
```
