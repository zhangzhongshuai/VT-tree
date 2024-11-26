#!/bin/bash

# 定义输入文件目录和输出文件
input_dir="../test"
output_csv="output_results.csv"

# 定义常量n（运行次数）
n=1  # 可以根据需要调整此值

# 清空输出文件内容（如果存在）
> "$output_csv"

# 写入CSV文件的表头
findt_header=$(printf "findt_%s," $(seq 1 $n))
cleart_header=$(printf "cleart_%s," $(seq 1 $n))
echo "${findt_header} ,${cleart_header}" >> "$output_csv"

# 定义输入文件的后缀数组
input_suffixes=(100 200 500 800 1000 2000)

# 遍历数组中的每个后缀
for suffix in "${input_suffixes[@]}"; do
    input_file="$input_dir/$suffix.txt"
    
    # 检查输入文件是否存在
    if [ -f "$input_file" ]; then
        findt_values=()
        cleart_values=()
        
        # 运行n次main.exe并记录每次的时间
        for run in $(seq 1 $n); do
            output=$(./main < "$input_file")
            
            # 提取findt和cleart的值
            findt=$(echo "$output" | grep -oP '(?<=findt = )\d+(\.\d+)?(?=s)')
            cleart=$(echo "$output" | grep -oP '(?<=cleart = )\d+(\.\d+)?(?=s)')
            
            # 存储提取的值
            findt_values+=("$findt")
            cleart_values+=("$cleart")
            
            # 向控制台打印消息
            echo "Processed input file: $input_file (Run #$run)"
        done
        
        # 将结果格式化并写入到CSV文件中
        findt_row=$(printf "%s," "${findt_values[@]}")
        cleart_row=$(printf "%s," "${cleart_values[@]}")
        echo "${findt_row} ,${cleart_row}" >> "$output_csv"
        
    else
        echo "Input file not found: $input_file"
    fi
done

echo "All tasks completed. Results are saved in $output_csv."
