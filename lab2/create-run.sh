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
IMAGE_NAME="$MY_STUDENT_ID-worker-image"
VOLUME_NAME="$MY_STUDENT_ID-shared-data"
MOUNT_POINT="/shared"
CONTAINER_PREFIX="$MY_STUDENT_ID-worker"

docker volume create "$VOLUME_NAME" >/dev/null
echo "$VOLUME_NAME volume created successfully."

for ((i = 1; i <= CONTAINERS_COUNT; ++i)); do
    CONTAINER_NAME="$CONTAINER_PREFIX-$i"
    docker run --name "$CONTAINER_NAME" -d \
        -v "$VOLUME_NAME:$MOUNT_POINT" \
        "$IMAGE_NAME" >/dev/null
    echo "$CONTAINER_NAME started."
done
echo "$CONTAINERS_COUNT containers started successfully."
