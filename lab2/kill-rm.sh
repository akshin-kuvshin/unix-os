#!/usr/bin/env bash
set -Eeuo pipefail

# author: Danila "akshin_" Axyonov
# -------------------------

if [[ $# -ne 1 || ! "$1" =~ ^[1-9][0-9]*$ ]]; then
    echo "Usage: $0 [NUMBER_OF_CONTAINERS]" >&2
    exit 1
fi
CONTAINERS_COUNT="$1"

MY_STUDENT_ID="738345"
CONTAINER_PREFIX="$MY_STUDENT_ID-worker"
VOLUME_NAME="$MY_STUDENT_ID-shared-data"

for ((i = 1; i <= CONTAINERS_COUNT; ++i)); do
    CONTAINER_NAME="$CONTAINER_PREFIX-$i"
    docker kill "$CONTAINER_NAME" >/dev/null
    docker rm "$CONTAINER_NAME" >/dev/null
    echo "$CONTAINER_NAME killed and removed."
done
echo "$CONTAINERS_COUNT containers killed and removed successfully."

docker volume rm "$VOLUME_NAME" >/dev/null
echo "$VOLUME_NAME volume removed successfully."
