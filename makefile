include include/ef.gy/base.mk

NAME:=libefgy
BASE:=ef.gy
VERSION:=8

http-hello src/http-hello.cpp: include/asio.hpp
