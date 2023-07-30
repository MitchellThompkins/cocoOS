#!/bin/bash
set -e

echo "Configuring User=${USER} with MY_GID=${MY_GID} and MY_UID=${MY_UID}"

groupadd -g ${MY_GID} ${USER}
adduser --uid ${MY_UID} --gid ${MY_GID} ${USER}

chown -R ${USER}.${USER} /home/${USER}

su ${USER} --session-command "$@"
