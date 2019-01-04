#!/bin/bash

if [[ $# -ne 1 ]]
then
    echo "Please specify database name!"
    exit 1
fi

dbname=$1

# create database
./dbcreate $dbname

# create table 'nation'
echo "CREATE TABLE NATION (N_NATIONKEY int, N_NAME c25, N_REGIONKEY int, N_COMMENT s152);" | ./thisadb $dbname
echo "load NATION(\"/home/lcy/Documents/thisadb/data/nation.tbl\");" | ./thisadb $dbname

