# ThisaDB

This is a DataBase.

# Building

# Dependencies

ThisaDB use Gnu Bison(compatible with yacc) and flex to parser SQL, so you should has both of them.

In CentOS, you can install them using following commands:

```
# yum -y install bison flex 
```
In Ubuntu or debian, you can install them using following commands:

```
# sudo apt-get install bison flex
```
You should also insall ctags:

In CentOS:

```
# yum -y install ctags 
```

In Ubuntu or debian:

```
# sudo apt-get install ctags 
```

After installing these dependencies, you need create some dirs:

```
mkdir bin build lib
```
And now, you can build thisadb:

```
make
```

# Running

Firstly, you should use `./dbcreate <your database name>` to create your database and then start ThisaDB:

```
$ ./thisadb <your database name>
```
# Declaration

ThisaDB is based on [redbase](https://github.com/junkumar/redbase).
