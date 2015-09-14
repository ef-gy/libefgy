include include/ef.gy/base.mk

NAME:=libefgy
BASE:=ef.gy
VERSION:=8

server src/server.cpp fortuned src/fortuned.cpp: include/asio.hpp
