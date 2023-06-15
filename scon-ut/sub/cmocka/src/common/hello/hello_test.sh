#!/bin/bash
#STUBS+=("func_name")

STUB_WRAPS=${STUBS[@]/#/\-Wl,\-\-wrap=}

shift 1

$* $STUB_WRAPS
