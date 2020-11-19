#!/bin/bash

ID=${1:-789}

aozora content --id $ID | perl -pe '$_="" if $. < 24 or /^底本：/ .. eof;s/《.*?》/$1/g;s/｜//g;s/［＃.*?］//g' > data/aozora-$ID.txt