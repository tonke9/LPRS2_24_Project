#!/bin/bash

exit 0

./waf configure

# Housekeeper.
./waf build upload --app=04 

./waf debug_uart --port=0
