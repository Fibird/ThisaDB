#!/bin/bash

if [[ $# -ne 1 ]]
then
    echo "Please specify database name!"
    exit 1
fi

dbname=$1

# create database
./dbcreate $dbname

# create table 'NATION' and load data
echo "CREATE TABLE NATION (N_NATIONKEY int, N_NAME c25, N_REGIONKEY int, N_COMMENT s152);" | ./thisadb $dbname
echo "LOAD NATION (\"/home/ubuntu/ThisaDB/data/nation.tbl\");" | ./thisadb $dbname

# create table 'REGION' and load data
echo "CREATE TABLE REGION (R_REGIONKEY int, R_NAME c25, R_COMMENT s152);" | ./thisadb $dbname
echo "LOAD REGION (\"/home/ubuntu/ThisaDB/data/region.tbl\");" | ./thisadb $dbname

# create table 'PART' and load data
echo "CREATE TABLE PART (P_PARTKEY int, P_NAME s55, P_MFGR c25, P_BRAND c10, P_TYPE s25, P_SIZE int, P_CONTAINER c10, P_RETAILPRICE float, P_COMMENT s23);" | ./thisadb $dbname
echo "LOAD PART (\"/home/ubuntu/ThisaDB/data/part.tbl\");" | ./thisadb $dbname

# create table 'SUPPLIER' and load data
echo "CREATE TABLE SUPPLIER (S_SUPPKEY int, S_NAME c25, S_ADDRESS s40, S_NATIONKEY int, S_PHONE c15, S_ACCTBAL float, S_COMMENT s101);" | ./thisadb $dbname
echo "LOAD SUPPLIER (\"/home/ubuntu/ThisaDB/data/supplier.tbl\");" | ./thisadb $dbname

# create table 'PARTSUPP' and load data
echo "CREATE TABLE PARTSUPP (PS_PARTKEY int, PS_SUPPKEY int, PS_AVAILQTY	int, PS_SUPPLYCOST float, PS_COMMENT s199);" | ./thisadb $dbname
echo "LOAD PARTSUPP (\"/home/ubuntu/ThisaDB/data/partsupp.tbl\");" | ./thisadb $dbname

# create table 'CUSTOMER' and load data
echo "CREATE TABLE CUSTOMER (C_CUSTKEY int, C_NAME s25, C_ADDRESS s40, C_NATIONKEY int, C_PHONE c15, C_ACCTBAL float, C_MKTSEGMENT c10, C_COMMENT s117);" | ./thisadb $dbname
echo "LOAD CUSTOMER (\"/home/ubuntu/ThisaDB/data/customer.tbl\");" | ./thisadb $dbname

# create table 'ORDERS' and load data
echo "CREATE TABLE ORDERS (O_ORDERKEY int, O_CUSTKEY int, O_ORDERSTATUS c1, O_TOTALPRICE float, O_ORDERDATE d, O_ORDERPRIORITY c15, O_CLERK c15, O_SHIPPRIORITY int, O_COMMENT s79);" | ./thisadb $dbname
echo "LOAD ORDERS (\"/home/ubuntu/ThisaDB/data/orders.tbl\");" | ./thisadb $dbname

# create table 'LINEITEM' and load data
echo "CREATE TABLE LINEITEM (L_ORDERKEY int, L_PARTKEY int, L_SUPPKEY int, L_LINENUMBER int, L_QUANTITY float, L_EXTENDEDPRICE float, L_DISCOUNT float, L_TAX float, L_RETURNFLAG c1, L_LINESTATUS c1, L_SHIPDATE d, L_COMMITDATE d, L_RECEIPTDATE d, L_SHIPINSTRUCT c25, L_SHIPMODE c10, L_COMMENT s44);" | ./thisadb $dbname
echo "LOAD LINEITEM (\"/home/ubuntu/ThisaDB/data/lineitem.tbl\");" | ./thisadb $dbname

