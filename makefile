include include/ef.gy/base.mk

NAME:=libefgy
BASE:=ef.gy
VERSION:=8

server src/server.cpp: include/asio.hpp
