#!/usr/bin/env bash
set -Eeuo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
IMAGE_NAME="osc-fedora"
CONTAINER_NAME="osc-container"
DATA_DIR="$SCRIPT_DIR"

usage() {
    cat <<EOF
Usage: $0 {create|enter|help}

  create  Build the image and create the permanent container
  enter   Start or enter the existing container
EOF
}

create_container() {
    echo "Building $IMAGE_NAME with Podman..."
    podman build --tag "$IMAGE_NAME" "$SCRIPT_DIR"

    if podman container inspect "$CONTAINER_NAME" >/dev/null 2>&1; then
        echo "Replacing existing container $CONTAINER_NAME..."
        podman rm --force "$CONTAINER_NAME" >/dev/null
    fi

    echo "Creating permanent container $CONTAINER_NAME..."
    echo "Shared directory: $DATA_DIR -> /root"

    # :Z assigns a private SELinux label to the bind-mounted directory.
    podman create --interactive --tty \
        --name "$CONTAINER_NAME" \
        --volume "$DATA_DIR:/root:Z" \
        "$IMAGE_NAME"
}

enter_container() {
    if ! podman container inspect "$CONTAINER_NAME" >/dev/null 2>&1; then
        echo "Container $CONTAINER_NAME does not exist. Run '$0 create' first." >&2
        exit 1
    fi

    running="$(podman container inspect --format '{{.State.Running}}' "$CONTAINER_NAME")"

    if [[ "$running" == "true" ]]; then
        echo "Attaching to existing container $CONTAINER_NAME..."
        exec podman attach --interactive "$CONTAINER_NAME"
    else
        echo "Starting existing container $CONTAINER_NAME..."
        exec podman start --attach --interactive "$CONTAINER_NAME"
    fi
}

command="${1:-enter}"

case "$command" in
    create)
        create_container
        ;;
    enter|start)
        enter_container
        ;;
    help|-h|--help)
        usage
        ;;
    *)
        echo "Unknown command: $command" >&2
        usage >&2
        exit 2
        ;;
esac
