######################################################3
# Stage 1 Build static frontend assets served by app
######################################################3

FROM node:24-alpine3.18 AS build

SHELL [ "/bin/sh", "-c", "-l" ]

ENV USER=node
ENV HOME=/home/$USER
ENV PROJECT_NAME=rgs
ENV PROJECT_TOP=$HOME/$PROJECT_NAME
ENV APP_ROOT=static/game
ENV GAME_LIB_ROOT=static/slot-game-library

# overriden during build
ARG GAME_PROXY_PREFIX=/
ARG GAME_LIB_CONFIG_URL
ENV GAME_PROXY_PREFIX=${GAME_PROXY_PREFIX}
ENV GAME_LIB_CONFIG_URL=${GAME_LIB_CONFIG_URL}

RUN echo ${GAME_LIB_CONFIG_URL}

# Copy source
WORKDIR $PROJECT_TOP
COPY --chown=$USER:$USER package*.json                .
COPY --chown=$USER:$USER $APP_ROOT                    $APP_ROOT
COPY --chown=$USER:$USER $GAME_LIB_ROOT/package.json  $PROJECT_TOP/$GAME_LIB_ROOT/package.json

# Copy dependencies
COPY --chown=$USER:$USER $GAME_LIB_ROOT               $PROJECT_TOP/$GAME_LIB_ROOT

# Install dependencies and build
RUN npm i
RUN npm run build

# #############################################################3
# # Stage 2 Build app img copying build assets from build img
# #############################################################3

FROM nginx:1.25-alpine AS final

ENV USER=node
ENV HOME=/home/$USER
ENV PROJECT_NAME=rgs
ENV PROJECT_TOP=$HOME/$PROJECT_NAME
ENV APP_ROOT=static/game

# Copy from build first to use cache before npm i
# COPY --from=build $PROJECT_TOP/$APP_ROOT/dist $APP_ROOT/dist

# Set working directory
WORKDIR /usr/share/nginx/html

# Copy build files from buld stage to nginx static html dir
COPY --from=build --chown=$USER:$USER $PROJECT_TOP/$APP_ROOT/dist/. .

# Copy nginx config to serve the static files
COPY docker/static/game/nginx.conf /etc/nginx/templates/default.conf.template
