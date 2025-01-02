import itertools

fixed = ["111"]
others = ["110", "101", "011", "100", "010", "001", "000"]

all_permutations = list(itertools.permutations(others))
sorted_combinations = [fixed + list(permutation) for permutation in all_permutations]

fit_combinations = []

# 只查看前 1 組 combination（也就是第 0 組），以示範為主
for combination_idx, combination in enumerate(sorted_combinations, start=1):
    fit = 0
    # print(f"=== Combination #{combination_idx} ===")

    found_item = None  # 用於紀錄「排名最高且 item[1] == '0'」的字串
    for item in combination:        
        # 檢查 item[1] 是否為 '0'
        if item[1] == '0':
            found_item = item
            # 找到第一個符合的就 break，確保這個就是「排名最高」的
            break

    if found_item is not None:
        # 判斷 item[0]：決定「1 指向 0」或「1 不指向 0」
        if found_item[0] == '0':
            fit += 0
            # print(f"1 指向 0")
        else:
            fit += 1
            # print(f"1 不指向 0")

    found_item = None  # 用於紀錄「排名最高且 item[1] == '0'」的字串
    for item in combination:        
        # 檢查 item[1] 是否為 '0'
        if item[2] == '0':
            found_item = item
            # 找到第一個符合的就 break，確保這個就是「排名最高」的
            break

    if found_item is not None:
        # 判斷 item[0]：決定「1 指向 0」或「1 不指向 0」
        if found_item[0] == '0':
            fit += 0
            # print(f"1 指向 0")
        else:
            fit += 1
            # print(f"1 不指向 0")

    if fit == 2:
        fit_combinations = fit_combinations + [combination]

# print(len(fit_combinations))
###############################################################################
# for combination_idx, combination in enumerate(fit_combinations, start=1):
#     print(f"=== Fit #{combination_idx} ===")

#     found_item = None  # 用於紀錄「排名最高且 item[1] == '0'」的字串
#     for item in combination:
#         print(item)  # 先列出該 combination 裡的所有項目（排名順序）
###############################################################################


# import itertools

# fixed = ["111"]
# others = ["110", "101", "011", "100", "010", "001", "000"]

# all_permutations = list(itertools.permutations(others))
# fit_combinations = [fixed + list(permutation) for permutation in all_permutations]

no_2_epi = []
for combination_idx, combination in enumerate(fit_combinations, start=1):

    # 建立一個字典，用來記錄在該 combination 中
    # "00", "01", "10", "11" 四種 pattern 的「第一個(最高排名)」item
    found_items = {
        "00": None,  # 對應 item[1] == '0' and item[2] == '0'
        "01": None,  # 對應 item[1] == '0' and item[2] == '1'
        "10": None,  # 對應 item[1] == '1' and item[2] == '0'
        "11": None   # 對應 item[1] == '1' and item[2] == '1'
    }

    # 依排名順序遍歷每個 item
    for item in combination:
        # 取出中間兩碼(第 1, 2 位) → 用於比對 "00"/"01"/"10"/"11"
        pattern = item[1] + item[2]

        # 若這個 pattern 還沒有找到第一個符合的 item，就把它記起來
        if found_items[pattern] is None:
            found_items[pattern] = item

        # 如果四種 pattern 都已經找到，就可以提前結束這個 combination 的搜尋
        if all(x is not None for x in found_items.values()):
            break
    
    if (found_items["00"][0] != found_items["10"][0] and found_items["00"][0] != found_items["01"][0])\
    or (found_items["11"][0] != found_items["10"][0] and found_items["11"][0] != found_items["01"][0])\
    or (found_items["01"][0] != found_items["11"][0] and found_items["01"][0] != found_items["00"][0])\
    or (found_items["10"][0] != found_items["11"][0] and found_items["10"][0] != found_items["00"][0]):
        no_2_epi = no_2_epi 
    else:
        no_2_epi = no_2_epi + [combination]

print(len(no_2_epi))
###############################################################################
for combination_idx, combination in enumerate(no_2_epi, start=1):
    print(f"=== no_2_epi #{combination_idx} ===")

    found_item = None  # 用於紀錄「排名最高且 item[1] == '0'」的字串
    for item in combination:
        print(item)  # 先列出該 combination 裡的所有項目（排名順序）
###############################################################################