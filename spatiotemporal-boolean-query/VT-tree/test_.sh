#!/bin/bash

# 定义输出文件
output_file="output_results.txt"

# 清空输出文件内容（如果存在）
> "$output_file"

# 遍历第一个参数的取值范围 [0.0, 1]，步长为0.1
for param1 in $(seq 0.5 0.1 0.9); do
    # 遍历第二个参数的取值范围 [1, 9]
    for param2 in {1..7}; do
        # 遍历第三个参数的取值范围 [3, 15]
        for param3 in $(seq 3 1 15); do
            # 第四个参数固定为1000
            #param3=5
            param4=1000
            > "tmp.txt"
            echo "$param1 $param2 $param3 $param4" >> "tmp.txt"

            # 运行main.exe并获取输出的最后一行
            #last_line=$(./main < "$param1" "$param2" "$param3" "$param4" | tail -n 1)
            last_line=$(./main < "tmp.txt" | tail -n 1)
            
            # 提取时间值
            time=$(echo "$last_line" | awk '{print $5}')
            
            # 追加格式化结果到输出文件
            echo "param1 = $param1 param2 = $param2 param3 = $param3 param4 = $param4 time = $time"
            
            # 追加格式化结果到输出文件
            echo "param1 = $param1 param2 = $param2 param3 = $param3 param4 = $param4 time = $time" >> "$output_file"
            
            # 向控制台打印消息
            echo "Processed with params: $param1 $param2 $param3 $param4"
        done
    done
done

echo "All tasks completed. Results are saved in $output_file."
