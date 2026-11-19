#!/usr/bin/env bash
set -Eeuo pipefail

# author: Danila "akshin_" Axyonov
# -------------------------

MY_STUDENT_ID="738345"
IMAGE_NAME="$MY_STUDENT_ID-worker-image"

docker build --pull --no-cache -t "$IMAGE_NAME" . &>/dev/null

echo "$IMAGE_NAME built successfully."
