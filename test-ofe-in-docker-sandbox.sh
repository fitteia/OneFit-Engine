#!/usr/bin/env bash
set -euo pipefail

usage() {
cat <<EOF
Usage:
  $0 build debian|ubuntu|fedora|arch|suse|centos|all
  $0 run   debian|ubuntu|fedora|arch|suse|centos
  $0 shell debian|ubuntu|fedora|arch|suse|centos

Dockerfile lookup order:
  ./<distro>.dockerfile
  ./<distro>.Dockerfile
  ./Dockerfiles/<distro>.dockerfile
  ./Dockerfiles/<distro>.Dockerfile

Container layout:
  /home/ofe/.local   private container filesystem; OFE may be installed here
  /home/ofe/work     shared host work folder

Environment:
  HOST_WORK   Host directory mounted as /home/ofe/work
              default: $HOME/Downloads/sandbox
  PORT        Host port mapped to container 8142
              default: 8142
  KEEP=1      Keep container after exit
EOF
exit 2
}

cmd=${1:-}
distro=${2:-}
[[ -n "$cmd" && -n "$distro" ]] || usage

case "$distro" in
  debian|ubuntu|fedora|arch|suse|centos|all) ;;
  *) echo "Unknown distro: $distro" >&2; usage ;;
esac

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
host_work=${HOST_WORK:-"$HOME/Downloads/sandbox"}
host_work=$(mkdir -p "$host_work"; cd "$host_work" && pwd)
host_uid=$(id -u)
host_gid=$(id -g)
port=${PORT:-8142}

image_for() {
    printf "ofe-%s:latest" "$1"
}

dockerfile_for() {
    local d="$1"
    local candidates=(
        "$script_dir/$d.dockerfile"
        "$script_dir/$d.Dockerfile"
        "$script_dir/Dockerfiles/$d.dockerfile"
        "$script_dir/Dockerfiles/$d.Dockerfile"
    )

    local f
    for f in "${candidates[@]}"; do
        if [[ -f "$f" ]]; then
            printf "%s\n" "$f"
            return 0
        fi
    done

    return 1
}

print_missing_dockerfile() {
    local d="$1"

    echo "Cannot find a Dockerfile for '$d'." >&2
    echo "Searched:" >&2
    echo "  $script_dir/$d.dockerfile" >&2
    echo "  $script_dir/$d.Dockerfile" >&2
    echo "  $script_dir/Dockerfiles/$d.dockerfile" >&2
    echo "  $script_dir/Dockerfiles/$d.Dockerfile" >&2
}

build_one() {
    local d="$1"
    local df
    local image

    if ! df=$(dockerfile_for "$d"); then
        print_missing_dockerfile "$d"
        exit 1
    fi

    image=$(image_for "$d")

    echo "======================================================"
    echo "Distribution : $d"
    echo "Dockerfile   : $df"
    echo "Context      : $script_dir"
    echo "Image        : $image"
    echo "Host UID:GID : $host_uid:$host_gid"
    echo "======================================================"

    if docker buildx version >/dev/null 2>&1; then
        echo "Builder      : buildx"
        docker buildx build \
            --progress=plain \
            --load \
            --build-arg HOST_UID="$host_uid" \
            --build-arg HOST_GID="$host_gid" \
            -f "$df" \
            -t "$image" \
            "$script_dir"
    else
        echo "Builder      : legacy"
        DOCKER_BUILDKIT=0 docker build \
            --build-arg HOST_UID="$host_uid" \
            --build-arg HOST_GID="$host_gid" \
            -f "$df" \
            -t "$image" \
            "$script_dir"
    fi
}

run_one() {
    local d="$1"
    local image
    local name
    image=$(image_for "$d")
    name="ofe-$d"

    local rmarg=(--rm)
    if [[ "${KEEP:-0}" == "1" ]]; then
        rmarg=()
        docker rm -f "$name" >/dev/null 2>&1 || true
    fi

    echo "======================================================"
    echo "Container       : $name"
    echo "Image           : $image"
    echo "Host work       : $host_work"
    echo "Container work  : /home/ofe/work"
    echo "Private OFE dir : /home/ofe/.local"
    echo "Port            : $port -> 8142"
    echo "======================================================"

    docker run "${rmarg[@]}" -it \
        --name "$name" \
        -p "$port:8142" \
        -e HOST_UID="$host_uid" \
        -e HOST_GID="$host_gid" \
        --mount "type=bind,source=$host_work,target=/home/ofe/work" \
        -w /home/ofe/work \
        "$image" \
        /bin/bash
}

shell_one() {
    local d="$1"
    docker exec -it "ofe-$d" /bin/bash
}

case "$cmd" in
    build)
        if [[ "$distro" == all ]]; then
            for d in debian ubuntu fedora arch suse centos; do
                build_one "$d"
            done
        else
            build_one "$distro"
        fi
        ;;
    run)
        [[ "$distro" != all ]] || usage
        run_one "$distro"
        ;;
    shell|exec)
        [[ "$distro" != all ]] || usage
        shell_one "$distro"
        ;;
    *)
        usage
        ;;
esac
