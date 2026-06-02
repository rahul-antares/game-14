FROM node:24-alpine as build

RUN apk add --no-cache libc6-compat alpine-sdk cmake

ARG USER=node
ARG HOME=/home/$USER
ARG PROJECT_NAME=game
ARG SERVICE=service
ARG PROJECT_TOP=/home/$USER/$PROJECT_NAME
ARG SERVICE_TOP=$PROJECT_TOP/$SERVICE

USER $USER
RUN mkdir -p $PROJECT_TOP

# Set to production environment
ENV NODE_ENV dev
ARG VARIANT="96"
ENV VARIANT=${VARIANT}
ARG ALLOW_CHEATS=no
ENV ALLOW_CHEATS=${ALLOW_CHEATS}
ARG RNG_VERSION
ENV RNG_VERSION=${RNG_VERSION}

WORKDIR $PROJECT_TOP/

# Copy package*.json to use docker cache for npm ci
COPY --chown=$USER:$USER package*.json .
COPY --chown=$USER:$USER $SERVICE/package.json $SERVICE_TOP/package.json
COPY --chown=$USER:$USER core/package.json     $PROJECT_TOP/core/package.json

RUN npm i

# Copy source code
COPY --chown=$USER:$USER $SERVICE/. $SERVICE_TOP/.

COPY --chown=$USER:$USER core/CMakeLists.txt $PROJECT_TOP/core/CMakeLists.txt
COPY --chown=$USER:$USER core/cmake/BuildGame.cmake $PROJECT_TOP/core/cmake/BuildGame.cmake
COPY --chown=$USER:$USER core/index*.js $PROJECT_TOP/core/
COPY --chown=$USER:$USER core/scripts $PROJECT_TOP/core/scripts
COPY --chown=$USER:$USER core/src $PROJECT_TOP/core/src

COPY --chown=$USER:$USER rng/certified/librngapi-local.so.$RNG_VERSION $PROJECT_TOP/rng/core/build/lib/librngapi-local.so
COPY --chown=$USER:$USER rng/core/src/.     $PROJECT_TOP/rng/core/src/.

WORKDIR $PROJECT_TOP/core

#
# If build time variable "ALLOW_CHEATS" is "yes" then copy the binary
# with the cheats to "build/Release/game.node" (which is what the 
# "games/game1/index.js"
RUN echo "allow cheats = $ALLOW_CHEATS" > .note;
RUN echo "ALLOW_CHEATS = $ALLOW_CHEATS"
RUN echo "VARIANT = $VARIANT"

RUN if [ "$ALLOW_CHEATS" = "yes" ]; then \
        if [ "$VARIANT" = "96" ]; then \
            npm run build -- --target game96-wc && mv index96-wc.js index.js; \
        elif [ "$VARIANT" = "94" ]; then \
            npm run build -- --target game94-wc && mv index94-wc.js index.js; \
        else \
            npm run build -- --target game92-wc && mv index92-wc.js index.js; \
        fi; \
    else \
        if [ "$VARIANT" = "96" ]; then \
            npm run build -- --target game96 && mv index96.js index.js; \
        elif [ "$VARIANT" = "94" ]; then \
            npm run build -- --target game94 && mv index94.js index.js; \
        else \
            npm run build -- --target game92 && mv index92.js index.js; \
        fi; \
    fi

RUN rm -rf $PROJECT_TOP/rng

WORKDIR $SERVICE_TOP/

#
# Ensure that the game engine uses the mounted
# library
#
ENV LD_LIBRARY_PATH=/rng/lib:$LD_LIBRARY_PATH

CMD [ "node", "./index.js" ]
