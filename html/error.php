<?php
$interval=60; //minutes
set_time_limit(0);

while (1){
    $now=time();
    #do the routine job, trigger a php function and what not.
    sleep($interval*60-(time()-$now));
} ?>