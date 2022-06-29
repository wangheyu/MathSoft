#!/bin/bash
#
#---------------------------------------
#Author:                    MartinHe
#Date:                      2019-03-20
#FileName:                  argsum.sh
#URL:                       https://blog.51cto.com/5033330 
#Desciption:                统计文件的空行数.
#Copyright(c)               2019 All rights reserved
#---------------------------------------

# 如果参数个数($#)小于(-lt)1 
if [ $# -lt 1 ];then
    echo "At least one argument(must be file)."
    # 非正常退出
    exit 10
# 检测目标文件($1)是否存在(-e), 并且(&&)检测目标文件是否可读(-f)
elif [ -e $1 ] && [ -f $1 ];then
    # 过滤(grep)空行(^$, ^表示开头, $表示结尾)
    # 然后数一下行数(wc -l)
    Space_Line=`grep "^$" $1 | wc -l`
    # 输出, 这里${Space_Line}的{}加不加都行.
    echo "the $1 has ${Space_Line} space line"                        
fi
