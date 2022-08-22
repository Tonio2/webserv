#!/bin/bash
for i in {1..100}
do
    curl http://localhost:80
done

for i in {1..100}
do
    curl --data "key1=value1" http://localhost:80/html/test.php
done