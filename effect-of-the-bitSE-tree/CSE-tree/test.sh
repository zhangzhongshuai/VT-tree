#!/bin/bash

# 定义输入文件目录和输出文件
input_dir="../test"
output_file="output_results.txt"

# 清空输出文件内容（如果存在）
> "$output_file"

# 定义输入文件的后缀数组
input_suffixes=(2000 4000 6000 8000 10000 12000 14000 16000 18000 20000)

# 遍历数组中的每个后缀
for suffix in "${input_suffixes[@]}"; do
    input_file="$input_dir/$suffix.txt"
    
    # 检查输入文件是否存在
    if [ -f "$input_file" ]; then
        # 运行main.exe并获取输出的最后一行
        last_line=$(./main < "$input_file" | tail -n 1)
        
        # 提取时间值
        time=$(echo "$last_line" | awk '{print $5}')
        
        # 追加格式化结果到输出文件
        echo "n = $suffix time = $time" >> "$output_file"
        
        # 向控制台打印消息
        echo "Processed input file: $input_file"
    else
        echo "Input file not found: $input_file"
    fi
done

echo "All tasks completed. Results are saved in $output_file."
