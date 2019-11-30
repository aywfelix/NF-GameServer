# -*- coding: UTF-8 -*-
# 根据vcxproj生成cmake文件，便于linux下程序编译

import os
import sys
import xml.etree.ElementTree as ET
import codecs
import re
import json


class cmake_tool(object):
    def __init__(self):
        self.compile_files = []
        self.project = ""
        self.root = None  # xml parse
        self.cmake_conf = {}
        self.definitions = ""
        self.include_directories = ""
        self.link_directories = ""
        self.link_libraries = ""
        self.library = ""

    def writecmakefile(self, proj_path):
        cmake_str = ""
        cmake_str += 'cmake_minimum_required(VERSION 2.8)'
        cmake_str += '\n'
        cmake_str += 'project(%s)' % self.project
        cmake_str += '\n'
        cmake_str += 'add_definitions(%s)' % self.definitions
        cmake_str += '\n'
        cmake_str += 'include_directories(%s)' % self.include_directories
        cmake_str += '\n'
        cmake_str += 'link_directories(%s)' % self.link_directories
        cmake_str += '\n'
        cmake_str += 'set(SOURCE_FILE %s)' % " ".join(self.compile_files)
        cmake_str += '\n'
        cmake_str += '''
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -o2")
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
        \n'''
        if sys.argv[2] == "debug":
            cmake_str += 'set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")'
        cmake_str += '\n'
        if self.library == "":
            cmake_str += 'add_executable(%s ${SOURCE_FILE})' % self.project
            cmake_str += 'target_link_libraries(%s  %s)' % (
                self.project, self.link_libraries)
        else:
            cmake_str += 'add_library(%s %s ${SOURCE_FILE})' % (
                self.project, self.library)

        cmake_str += '\n'
        print(cmake_str)

        with open(proj_path + 'CMakeLists.txt', 'w') as f:
            f.write(cmake_str)
            f.flush()

    def readconf(self, proj_path):
        contents = ""
        with codecs.open(proj_path + "cmake_conf.json", "r", "utf-8") as f:
            contents = f.read()
        self.cmake_conf = json.loads(contents)
        # print(self.cmake_conf)
        # print(self.cmake_conf['debug'])
        if sys.argv[2] == "debug":
            self.definitions = self.cmake_conf['common']['definitions'] + \
                " " + self.cmake_conf['debug']['definitions']
            self.include_directories = self.cmake_conf['common']['include_directories']
            self.link_directories = self.cmake_conf['common']['link_directories'] + \
                " " + self.cmake_conf['debug']['link_directories']
            self.link_libraries = self.cmake_conf['common']['link_libraries'] + \
                " " + self.cmake_conf['debug']['link_libraries']
        else:
            self.definitions = self.cmake_conf['common']['definitions'] + \
                " " + self.cmake_conf['release']['definitions']
            self.include_directories = self.cmake_conf['common']['include_directories']
            self.link_directories = self.cmake_conf['common']['link_directories'] + \
                " " + self.cmake_conf['release']['link_directories']
            self.link_libraries = self.cmake_conf['common']['link_libraries'] + \
                " " + self.cmake_conf['release']['link_libraries']

        self.library = self.cmake_conf['common']['library']

    def loadxml(self, xml_file, proj_path):
        if not os.path.exists(proj_path):
            print("path not exist")
            return
        self.project = sys.argv[1]

        contents = ""
        with codecs.open(xml_file, "r", "utf-8") as f:
            contents = f.read()
        pattern = re.compile(r'xmlns=".*"', re.I)
        xml_contents = re.sub(pattern, "", contents)
        self.root = ET.fromstring(xml_contents)
        compile_files_item = self.root.findall('ItemGroup/ClCompile')
        for item in compile_files_item:
            for k, v in item.items():
                self.compile_files.append(v.replace('\\', '/'))

        self.readconf(proj_path)
        self.writecmakefile(proj_path)


if __name__ == '__main__':
    tool = cmake_tool()
    vcproj_file = "../"+sys.argv[1]+"/"+sys.argv[1]+".vcxproj"
    proj_path = "../"+sys.argv[1]+"/"
    tool.loadxml(vcproj_file, proj_path)
