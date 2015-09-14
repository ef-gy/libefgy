include include/ef.gy/base.mk

NAME:=libefgy
BASE:=ef.gy
VERSION:=8

# the two server examples rely on asio.hpp
server fortuned: include/asio.hpp
