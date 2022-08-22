#pragma once

//string
#include <iostream>
#include <string>
#include <sstream> 
#include <fstream>
#include <string.h>

//network
#include <netinet/in.h>
#include <arpa/inet.h>

//Standard containers
#include <map>
#include <vector>

//socket
#include <sys/socket.h>

//kqueue
#include <sys/event.h>

//stream
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

//error
#include <errno.h>

//define
#ifndef LOG_LVL
#define LOG_LVL 4
#endif
#define ERROR 4
#define ALERT 3
#define DEBUG 2
#define INFO 1

//helper functions
int is_not_space(int c) {
    return !std::isspace(c);
}

void triml(std::string & s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), is_not_space));
}

void trimr(std::string & s) {
    int i = s.size() - 1;
    while (std::isspace(s[i]))
        s.erase(i--, 1);
}

void trim(std::string & s) {
    trimr(s);
    triml(s);
}

std::string getline(std::string text, size_t & pos) {
    int new_pos = text.find("\n", pos);
    std::string ret = text.substr(pos, (new_pos - pos));
    pos = new_pos + 1;
    if (ret[ret.size() - 1] == '\r')
        ret.erase(ret.size() - 1);
    return ret;
}

std::string get_next_line(std::string text, size_t & pos) {
    if (pos == std::string::npos)
        return "";
    std::string ret;
    size_t new_pos = text.find("\n", pos);
    if (new_pos != std::string::npos)
    {
        ret = text.substr(pos, (new_pos - pos + 1));
        pos = new_pos + 1;
    }
    else
    {
        ret = text.substr(pos);
        pos = std::string::npos;
    }
    return ret;
}

std::string	ft_to_string(size_t n) {
	std::stringstream tmp;
	tmp << n;
	return tmp.str();
}

std::vector<std::string> ft_split(std::string str, char c) {
    std::vector<std::string>    ret;
    std::string                 word;
    int                         pos = 0;

    size_t len = str.find(c, pos);
    word = str.substr(pos, len);
    while (len != std::string::npos)
    {
        ret.push_back(word);
        pos = len + 1;
        len = str.find(c, pos);
        word = str.substr(pos, len - pos);
    }
    ret.push_back(word);
    return ret;
}