#!/bin/bash
set -e

echo "Configuring User=${USER} with MY_GID=${MY_GID} and MY_UID=${MY_UID}"

groupadd -g ${MY_GID} ${USER} || true
adduser --uid ${MY_UID} --gid ${MY_GID} ${USER} || true

chown -R ${MY_UID}:${MY_GID} /home/${USER}

su ${USER} --session-command "$@"
