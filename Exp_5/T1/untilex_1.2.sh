#!/bin/bash

until [ "$name" = "bye" ]; do
    read -p "to whom and how? " name method
    case $method in
        "1") echo "Hello $name!";;
        "2") echo "Ni Hao $name!";;
        *) echo "Go die $name!";;
    esac
done
