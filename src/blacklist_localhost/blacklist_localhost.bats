#!/usr/bin/env bats

setup() {
    export PATH="$BATS_TEST_DIRNAME:$PATH"
    export PORT=$((10000 + $RANDOM % 10000))

    export FLAG="-c"
    if uname -s | grep Darwin; then
    	export FLAG="-e"
    fi
}


@test "${BATS_TEST_DIRNAME##*/} blocks localhost" {
    nc -lp $PORT $FLAG "$BATS_TEST_DIRNAME/blacklist_localhost echo hi" &
    run nc localhost $PORT
    [ "$output" = "No connections from localhost" ]
}
