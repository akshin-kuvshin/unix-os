#!/usr/bin/env bash
set -Eeuo pipefail
shopt -s nullglob

# author: Danila "akshin_" Axyonov
# -------------------------

usage() {
    cat <<EOF
Usage:
    $0 [OPTIONS] [ROOT]

Compile all C source files (*.c) in ROOT.

Arguments:
    ROOT          root directory containing C source files
                  (default: current directory)

Options:
    -h, --help    show this help message and exit

The script can read the following parameters from C source files:
    // Compiler: compiler_name
    // Output: output_filename

If the parameters are not specified, the defaults are:
    Compiler:     clang
    Output:       main.out
EOF
}

ROOT="."
root_set=false

while (( $# > 0 )); do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;

        -*)
            printf 'Error: unknown option: %s\n' "$1" >&2
            printf 'Try "%s --help" for more information.\n' "$0" >&2
            exit 1
            ;;

        *)
            if "$root_set"; then
                printf 'Error: multiple ROOT directories specified.\n' >&2
                printf 'Try "%s --help" for more information.\n' "$0" >&2
                exit 1
            fi
            root_set=true

            ROOT="$1"
            if [[ ! -d "$ROOT" ]]; then
                printf 'Error: ROOT is not a directory: %s\n' "$ROOT" >&2
                exit 1
            fi
            ;;
    esac

    shift
done

# -------------------------

find_parameter() {
    for file in "$ROOT"/*.c; do
        parameter=$(sed -n "s#^// ${1}: ##Ip" "$file")

        if [[ -n "$parameter" ]]; then
            printf '\"%s\" parameter has been set to \"%s\" (found in %s).\n' "$1" "$parameter" "$file"
            return 0
        fi
    done

    return 1
}

compiler="clang"
if find_parameter "compiler"; then
    compiler="$parameter"
fi

output="main.out"
if find_parameter "output"; then
    output="$parameter"
fi

# -------------------------

tmpdir=$(mktemp -d)
trap 'rm -rf -- "$tmpdir"' EXIT

logfile="compile.log"
touch "$tmpdir/$logfile"

error_handler() {
    local status=$?
    printf '[ERROR] exit status: %d\n' "$status" >&2
    printf '[DETAILS] ' >&2
    cat "$tmpdir/$logfile" >&2
}
trap error_handler ERR

# -------------------------

for file in "$ROOT"/*.c; do
    "$compiler" -c "$file" -o "$tmpdir/$file.o" 2> "$tmpdir/$logfile"
done

"$compiler" "$tmpdir"/*.o -o "$ROOT/$output" 2> "$tmpdir/$logfile"

echo "Done."
