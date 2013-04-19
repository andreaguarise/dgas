#!/bin/bash

export SSM_HOME=/opt/apel/ssm
/opt/glite/sbin/dgas-sumrecords2gocSSMAggregate.pl -c $1
$SSM_HOME/bin/run-ssm