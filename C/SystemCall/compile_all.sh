#!/bin/bash

# 컴파일러 설정 (ARM64 크로스 컴파일러 경로)
CC=gcc

# 소스 코드 디렉토리 경로
SOURCE_DIR="./"

# 출력 디렉토리 경로
OUTPUT_DIR="./"

# SOURCE_DIR로 이동
cd "$SOURCE_DIR" || exit

# 디렉토리 내의 모든 .c 파일을 컴파일
for source_file in *.c; do
    if [ -f "$source_file" ]; then
        # .c 파일 확장자를 제거하여 실행 파일 이름 생성
        executable_name="${source_file%.c}"

        # 컴파일 및 실행 파일 생성
        $CC -o "$OUTPUT_DIR/$executable_name" "$source_file"
        if [ $? -eq 0 ]; then
            echo "컴파일 및 실행 파일 생성: $source_file -> $OUTPUT_DIR/$executable_name"
        else
            echo "컴파일 실패: $source_file"
        fi
    fi
done

echo "모든 파일 컴파일 완료"
