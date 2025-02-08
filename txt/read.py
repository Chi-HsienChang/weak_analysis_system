import argparse

def read_lines(file_path, start, end):
    """ 讀取檔案中從 start 行到 end 行的內容 """
    with open(file_path, "r", encoding="utf-8") as file:
        for i, line in enumerate(file, start=1):
            if start <= i <= end:
                print(line.strip())
            elif i > end:
                break  # 讀到終點行數後提前結束，提高效率

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="讀取指定範圍行數的 .txt 檔案")
    parser.add_argument("file", help="要讀取的 .txt 檔案")
    parser.add_argument("start", type=int, help="起始行數（從 1 開始）")
    parser.add_argument("end", type=int, help="終點行數")

    args = parser.parse_args()
    read_lines(args.file, args.start, args.end)
